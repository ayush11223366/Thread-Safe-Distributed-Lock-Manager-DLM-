import urllib.request
import concurrent.futures
import time

# Notice we are targeting your new 9999 port!
URL = "http://localhost:9999/lock?id=4&uid=1"

def hit_api():
    try:
        response = urllib.request.urlopen(URL)
        return response.read().decode('utf-8')
    except Exception as e:
        return str(e)

requests_to_make = 1000
print(f"Firing {requests_to_make} concurrent lock requests at Node 4...")

start_time = time.time()
success_count = 0
fail_count = 0

# Blast the server with 100 threads simultaneously
with concurrent.futures.ThreadPoolExecutor(max_workers=100) as executor:
    results = list(executor.map(lambda _: hit_api(), range(requests_to_make)))

for res in results:
    if "SUCCESS" in res:
        success_count += 1
    elif "FAIL" in res:
        fail_count += 1

print("====================================")
print(f"Time taken: {time.time() - start_time:.2f} seconds")
print(f"Successful Locks: {success_count}")
print(f"Failed Locks (Handled Safely by Mutex): {fail_count}")
print("====================================")