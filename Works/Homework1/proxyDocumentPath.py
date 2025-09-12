import datetime
import zmq
from defines import *


def main():
    if DO_LOG:
        print("Document path proxy started")
    ctx = zmq.Context()
    frontend = ctx.socket(zmq.PULL)
    backend = ctx.socket(zmq.PUSH)

    frontend.bind(DOCUMENT_PATH_FRONTEND_ADDRESS)
    backend.bind(DOCUMENT_PATH_BACKEND_ADDRESS)
    if DO_LOG:
        print("Document path proxy binded to hosts")
    messageProcessed = 0

    while True:
        try:
            message = frontend.recv_string()
        except KeyboardInterrupt:
            break
        if DO_LOG:
            print(datetime.datetime.now(), message)
        backend.send_string(message)
        messageProcessed += 1

    if DO_LOG:
        print(f"Document path proxy ended. Processed messages: {messageProcessed}")

if __name__ == "__main__":
    main()
