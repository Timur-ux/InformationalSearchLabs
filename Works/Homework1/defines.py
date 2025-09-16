from hashlib import md5

DOCUMENTS_QUANTITY_REQUIRED = 1e3 + 1
DOCUMENTS_STORAGE_PATH="/".join(__file__.split("/")[:-3]) + "/documents"

LOADER_START_ADDRESS="https://ria.ru/"

URL_FRONTEND_ADDRESS="tcp://localhost:15000"
URL_BACKEND_ADDRESS= "tcp://localhost:15001"

DOCUMENT_PATH_FRONTEND_ADDRESS="tcp://localhost:15003"
DOCUMENT_PATH_BACKEND_ADDRESS= "tcp://localhost:15002"

ACK_FRONTEND_ADDRESS="tcp://localhost:15004"
ACK_BACKEND_ADDRESS= "tcp://localhost:15005"

ADDRESS_BLACKLIST=["wikipedia"]

SUCCESS=b"SUCCESS"
FAILED=b"FAILED"

HASH_FUNC = md5
DO_LOG=True
