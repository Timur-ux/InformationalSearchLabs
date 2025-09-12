import datetime
import hashlib
import zmq
from defines import *


def main():
    if DO_LOG:
        print("Url proxy started")
    ctx = zmq.Context()
    frontend = ctx.socket(zmq.PULL)
    backend = ctx.socket(zmq.PUSH)

    frontend.bind(URL_FRONTEND_ADDRESS)
    backend.bind(URL_BACKEND_ADDRESS)
    if DO_LOG:
        print("Url proxy binded to hosts")
    messageProcessed = 0
    pagesHash = set()

    while True:
        try:
            message = frontend.recv_string()
        except KeyboardInterrupt:
            break
        if DO_LOG:
            print(datetime.datetime.now(), message)
        hash = HASH_FUNC(message.encode("utf-8")).hexdigest() 
        if hash in pagesHash:
            continue

        pagesHash.add(hash)
        backend.send_string(message)
        messageProcessed += 1

    if DO_LOG:
        print(f"Url proxy ended. Processed messages: {messageProcessed}")

if __name__ == "__main__":
    main()
