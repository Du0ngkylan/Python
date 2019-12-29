import hashlib

hash_object = hashlib.md5(b'Help me, please')
print(hash_object.hexdigest())