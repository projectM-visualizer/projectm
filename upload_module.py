
loc_file1 = "projectm-v.024-thread.1ijs" #@param ["sh4.1ijs", "sh5.1ijs", "g3007.wasm", "g3008.wasm", "g3009.wasm", "sh6.1ijs", "g3010.wasm"] {allow-input: true}
loc_file2 = "projectm-v.024-thread.wasm" #@param ["sh4.1ijs", "sh5.1ijs", "g3007.wasm", "g3008.wasm", "g3009.wasm", "sh6.1ijs", "g3010.wasm"] {allow-input: true}
import os
import urllib
import requests as reqs
import re
import paramiko
host = "1ink.us"
username  = "ford442"
password  = "GoogleBez12"
port = 22
file_name=loc_file1
local_path="/root/projectm/"+file_name
transport = paramiko.Transport((host, port))
destination_path="wasm.noahcohn.com/pm/"+file_name
destination_path2="noahcohn.com/pm/"+file_name
transport.connect(username = username, password = password)
sftp = paramiko.SFTPClient.from_transport(transport)
sftp.put(local_path, destination_path)
sftp.put(local_path, destination_path2)
sftp.close()
transport.close()
file_name2=loc_file2
local_path="/root/projectm/"+file_name2
transport = paramiko.Transport((host, port))
destination_path="wasm.noahcohn.com/pm/"+file_name2
destination_path2="noahcohn.com/pm/"+file_name2
transport.connect(username = username, password = password)
sftp = paramiko.SFTPClient.from_transport(transport)
sftp.put(local_path, destination_path)
sftp.put(local_path, destination_path2)
sftp.close()
transport.close()
loc_file3 = "projectm-v.024-thread.3ijs" #@param ["sh4.1ijs", "sh5.1ijs", "g3007.wasm", "g3008.wasm", "g3009.wasm", "sh6.1ijs", "g3010.wasm"] {allow-input: true}
import os
import urllib
import requests as reqs
import re
import paramiko
host = "1ink.us"
username  = "ford442"
password  = "GoogleBez12"
port = 22
file_name=loc_file3
local_path="/root/projectm/"+file_name
transport = paramiko.Transport((host, port))
destination_path="wasm.noahcohn.com/pm/"+file_name
transport.connect(username = username, password = password)
sftp = paramiko.SFTPClient.from_transport(transport)
sftp.put(local_path, destination_path)
sftp.close()
transport.close()