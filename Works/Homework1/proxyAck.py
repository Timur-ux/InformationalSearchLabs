import zmq
from defines import *


def main():
    if DO_LOG:
        print("Ack proxy started")
    ctx = zmq.Context()
    frontend = ctx.socket(zmq.XSUB)
    backend = ctx.socket(zmq.XPUB)

    frontend.bind(ACK_FRONTEND_ADDRESS)
    backend.bind(ACK_BACKEND_ADDRESS)

    if DO_LOG:
        print("Ack proxy binded to hosts")

    zmq.proxy(frontend, backend)


if __name__ == "__main__":
    main()
