import datetime
import zmq
import requests
import argparse

from defines import *


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
    sockUrl = ctx.socket(zmq.PULL)
    sockDocumentPath = ctx.socket(zmq.PUSH)
    sockAck = ctx.socket(zmq.PUB)

    sockUrl.connect(URL_BACKEND_ADDRESS)
    sockDocumentPath.connect(DOCUMENT_PATH_FRONTEND_ADDRESS)
    sockAck.connect(ACK_FRONTEND_ADDRESS)
    if DO_LOG:
        print("Loader connected to hosts")

    session = requests.session()

    start = datetime.datetime.now()
    documentsLoaded = 0
    try:
        while documentsLoaded < documentsRequired:
            url = sockUrl.recv_string()
            hash = HASH_FUNC(url.encode("utf-8")).hexdigest()
            page = loadDocument(session, url)

            if page is None:
                sockAck.send_multipart([b'status', FAILED])
                continue

            documentPath = f"{DOCUMENTS_STORAGE_PATH}/{hash}.html"
            with open(documentPath, "w") as file:
                file.write(page)

            documentsLoaded += 1
            sockAck.send_multipart([b'status', SUCCESS])
            sockDocumentPath.send_string(documentPath)
            if DO_LOG:
                print(f"{url} loaded")

    except KeyboardInterrupt:
        pass

    sockAck.send_multipart([b'exit', b""])

    end = datetime.datetime.now()
    timeElapsed = (end - start).seconds

    print(f"Done, loaded documents: {documentsLoaded}")
    print(f"Time elapsed: {timeElapsed // 60}m {timeElapsed % 60}s")


if __name__ == "__main__":
    main()
