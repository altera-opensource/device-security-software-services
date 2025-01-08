#  This project is licensed as below.
#
#  ***************************************************************************
#
#  Copyright 2020-2025 Altera Corporation. All Rights Reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#
#  1. Redistributions of source code must retain the above copyright notice,
#  this list of conditions and the following disclaimer.
#
#  2. Redistributions in binary form must reproduce the above copyright
#  notice, this list of conditions and the following disclaimer in the
#  documentation and/or other materials provided with the distribution.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
#  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
#  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#  ***************************************************************************

from Crypto.Cipher import PKCS1_OAEP, AES
from Crypto.Hash import SHA384
from Crypto.PublicKey import RSA
from Crypto.Random import get_random_bytes
from Modules.Utilities import int2bytes


class Encryptor:
    aes_len = 32
    iv_len = 12
    mac_len = 16
    key = None
    rsa_pub_key_pem = None

    def __init__(self, rsa_pub_key_pem):
        self.key = Encryptor.generate_aes_key()
        self.rsa_pub_key_pem = rsa_pub_key_pem.hex()

    def encrypt(self, data):
        encrypted_data = Encryptor.aes_gcm_encryption(self.key, data)
        return encrypted_data.hex()

    def get_encrypted_key(self):
        encrypted_key = Encryptor.rsa_encryption(self.rsa_pub_key_pem, self.key)
        return encrypted_key.hex()

    @staticmethod
    def rsa_encryption(rsa_pub_key_pem, data):
        key = RSA.importKey(bytes.fromhex(rsa_pub_key_pem))
        cipher = PKCS1_OAEP.new(key, hashAlgo=SHA384)
        cipher_text = cipher.encrypt(bytes.fromhex(data))
        return cipher_text

    @staticmethod
    def aes_gcm_encryption(key, data):
        iv = get_random_bytes(Encryptor.iv_len)
        cipher = AES.new(bytes.fromhex(key), AES.MODE_GCM, nonce=iv, mac_len=Encryptor.mac_len)
        ciphertext, tag = cipher.encrypt_and_digest(bytes.fromhex(data))
        result = bytearray()
        result.extend(int2bytes(len(iv)))
        result.extend(iv)
        result.extend(ciphertext)
        result.extend(tag)
        return result

    @staticmethod
    def generate_aes_key():
        return get_random_bytes(Encryptor.aes_len).hex()
