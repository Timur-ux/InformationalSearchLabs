import datetime
import zmq
import requests
import argparse

from bs4 import BeautifulSoup as bs, Tag

from defines import *

def isAllowed(url: str):
    for subString in ADDRESS_BLACKLIST:
        if subString in url:
            return False
    return True

def parseAndProcessPage(page: str, sockUrlOut: zmq.Socket) -> str | None:
    try:
        soup: Tag = bs(page, "html.parser")
    except Exception as e:
        print(f"Error: {str(e)}")
        return None
    refs = soup.find_all("a")
    for ref in refs:
        if not ref.has_attr("href"):
            continue
        href: str = ref["href"]
        if not href.startswith("http") or not isAllowed(href):
            continue
        sockUrlOut.send_string(href)
        print(f"Send url: {href}")
    return soup.text



def loadDocument(session: requests.Session, url: str) -> str | None:
    try:
        response = session.get(url, timeout=.5)
        if response.status_code != 200:
            if DO_LOG:
                print(f"Error: can't load page [{url}]")
            return None
        if not response.headers['Content-Type'].startswith("text/html"):
            return None
        return response.text
    except Exception as e:
        if DO_LOG:
            print(f"{url[:20]} : {str(e)}")
        return None


def main():
    if DO_LOG:
        print("Loader starts")
    parser = argparse.ArgumentParser(description="Document loader")
    parser.add_argument("n", type=int, default=1,
                        help="Workers count, required documents count will be divided by this number")
    args = parser.parse_args()
    n = args.n
    documentsRequired = DOCUMENTS_QUANTITY_REQUIRED // n

    ctx = zmq.Context()
    sockUrlIn = ctx.socket(zmq.PULL)
    sockUrlOut = ctx.socket(zmq.PUSH)
    sockAckOut = ctx.socket(zmq.PUB)
    sockAckIn = ctx.socket(zmq.SUB)

    sockUrlIn.connect(URL_BACKEND_ADDRESS)
    sockUrlOut.connect(URL_FRONTEND_ADDRESS)
    sockAckOut.connect(ACK_FRONTEND_ADDRESS)
    sockAckIn.connect(ACK_BACKEND_ADDRESS)

    sockUrlOut.send_string(LOADER_START_ADDRESS)

    sockAckIn.setsockopt(zmq.SUBSCRIBE, b"")

    poller = zmq.Poller()
    poller.register(sockUrlIn, zmq.POLLIN)
    poller.register(sockAckIn, zmq.POLLIN)

    if DO_LOG:
        print("Loader connected to hosts")

    session = requests.session()

    start = datetime.datetime.now()
    documentsLoaded = 0
    while documentsLoaded < documentsRequired:
        try:
            socks = dict(poller.poll())
        except KeyboardInterrupt:
            break
        if sockAckIn in socks:
            [status, message] = sockAckIn.recv_multipart()
            if status == SUCCESS:
                documentsLoaded += 1
        if sockUrlIn in socks:
            url = sockUrlIn.recv_string()
            hash = HASH_FUNC(url.encode("utf-8")).hexdigest()
            page = loadDocument(session, url)

            if page is None:
                sockAckOut.send_multipart([b'status', FAILED])
                continue
            if DO_LOG:
                print(f"Loaded: {url}")
            page = parseAndProcessPage(page, sockUrlOut)
            if page is not None:
                documentPath = f"{DOCUMENTS_STORAGE_PATH}/{hash}.html"
                with open(documentPath, "w") as file:
                    file.write(url + '\n')
                    file.write(page[:30000]) # first 30 kb in order to fit into 40G memory

                documentsLoaded += 1
                sockAckOut.send_multipart([b'status', SUCCESS])

    end = datetime.datetime.now()
    timeElapsed = (end - start).seconds

    print(f"Done, loaded documents: {documentsLoaded}")
    print(f"Time elapsed: {timeElapsed // 60}m {timeElapsed % 60}s")


if __name__ == "__main__":
    main()
