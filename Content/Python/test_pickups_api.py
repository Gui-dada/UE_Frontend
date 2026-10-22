import urllib.request
import json

url = "http://localhost:8080/api/pickups"
try:
    with urllib.request.urlopen(url, timeout=5) as resp:
        data = json.loads(resp.read().decode("utf-8"))
        length = len(data)
        print(f"Response: {data}")
        print(f"Length: {length}")
        assert length == 5, f"Expected 5, got {length}"
        print("PASS: length is 5")
except Exception as e:
    print(f"FAIL: {e}")
