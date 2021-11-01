import requests, json, base64

req = requests.get("http://127.0.0.1:3000/get/Designer")
if (req.status_code != 200): print("Something went wrong I know that much")

req_text = req.text
ZIP = json.loads(req_text)["file"]

with open("package.zip", 'bw') as f:
    f.write(ZIP)

print("File written.")