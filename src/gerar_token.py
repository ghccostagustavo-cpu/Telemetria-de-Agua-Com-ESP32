import base64, hmac, hashlib, time, urllib.parse

host = "HubIoTMonitoramentoAgua.azure-devices.net"
device = "Esp32MonitoramentoAgua1"
key = "NRl7aSFKkL7DYTOgAGz+ujmN7J+JUzoocM1r6B3pB5I="

expiry = int(time.time()) + 3600 * 24 * 30
resource = f"{host}/devices/{device}"
string_to_sign = f"{resource}\n{expiry}"

key_bytes = base64.b64decode(key)
signature = hmac.new(key_bytes, string_to_sign.encode(), hashlib.sha256).digest()
sig = urllib.parse.quote(base64.b64encode(signature).decode(), safe='')

token = f"SharedAccessSignature sr={urllib.parse.quote(resource, safe='')}&sig={sig}&se={expiry}"
print(token)