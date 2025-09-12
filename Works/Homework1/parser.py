from typing import List
import zmq
from bs4 import BeautifulSoup as bs, Tag
import argparse

from defines import *

def isAllowed(url: str):
    for subString in ADDRESS_BLACKLIST:
        if subString in url:
            return False
    return True

def parseAndProcessPage(documentPath: str, sockOut: zmq.Socket):
    page = None
    with open(documentPath, "r") as file:
        page = file.read()
    if page is None:
        return
    try:
        soup: Tag = bs(page, "html.parser")
    except Exception as e:
        # print(f"Error: {str(e)}")
        return
    refs = soup.find_all("a")
    for ref in refs:
        if not ref.has_attr("href"):
            continue
        href: str = ref["href"]
        if not href.startswith("http") or not isAllowed(href):
            continue
        sockOut.send_string(href)
        # print(f"Send url: {href}")

    # Text lasting
    with open(documentPath, "w") as file:
        file.write(soup.text)


def main():
    parser = argparse.ArgumentParser(description="Document parser")
    parser.add_argument("n", type=int, default=0,
                        help="Loaders count, parser will be stopped when recieve loader's stop message n times")
    args = parser.parse_args()
    n = args.n

    ctx = zmq.Context()
    sockDocumentPath = ctx.socket(zmq.PULL)
    sockUrl = ctx.socket(zmq.PUSH)
    sockAck = ctx.socket(zmq.SUB)

    sockDocumentPath.connect(DOCUMENT_PATH_BACKEND_ADDRESS)
    sockUrl.connect(URL_FRONTEND_ADDRESS)

    sockAck.connect(ACK_BACKEND_ADDRESS)
    sockAck.setsockopt(zmq.SUBSCRIBE, b"")

    poller = zmq.Poller()
    poller.register(sockDocumentPath, zmq.POLLIN)
    poller.register(sockAck, zmq.POLLIN)


    sockUrl.send_string(LOADER_START_ADDRESS)

    documentsLoaded = 0
    while documentsLoaded < DOCUMENTS_QUANTITY_REQUIRED and n > 0:
        try:
            socks = dict(poller.poll())
        except KeyboardInterrupt:
            break

        if sockDocumentPath in socks:
            documentPath = sockDocumentPath.recv_string()
            if DO_LOG:
                print(documentPath)
            parseAndProcessPage(documentPath, sockUrl)

        if sockAck in socks:
            [topic, message] = sockAck.recv_multipart()
            if DO_LOG:
                print(topic, message)
            if topic == b'exit':
                n -= 1
            elif topic == b'status' and message == SUCCESS:
                documentsLoaded += 1



if __name__ == "__main__":
    main()
