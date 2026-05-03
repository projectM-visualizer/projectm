#!/usr/bin/env python3
import paramiko
import os

host = "1ink.us"
username = "ford442"
password = "GoogleBez12!"
port = 22

files = [
    "projectm-v.024-thread.1ijs",
    "projectm-v.024-thread.wasm",
    "projectm-v.024-thread.3ijs",
]

local_dir = "/root/Project-M"
remote_bases = ["wasm.noahcohn.com/pm/", "noahcohn.com/pm/"]

for filename in files:
    local_path = os.path.join(local_dir, filename)
    if not os.path.isfile(local_path):
        print(f"Skipping missing file: {local_path}")
        continue

    for remote_base in remote_bases:
        remote_path = remote_base + filename
        try:
            transport = paramiko.Transport((host, port))
            transport.connect(username=username, password=password)
            sftp = paramiko.SFTPClient.from_transport(transport)
            print(f"Uploading {local_path} -> {remote_path}")
            sftp.put(local_path, remote_path)
            sftp.close()
            transport.close()
            print(f"  OK: {remote_path}")
        except Exception as e:
            print(f"  FAILED: {remote_path} - {e}")

print("Deploy complete.")
