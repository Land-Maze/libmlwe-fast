import ctypes
import hashlib
import os
import secrets
import sys

MLWE_FAST_PUBLICKEYBYTES = 800
MLWE_FAST_SECRETKEYBYTES = 1632
MLWE_FAST_INDCPA_SECRETKEYBYTES = 768
MLWE_FAST_CIPHERTEXTBYTES = 768
MLWE_FAST_SSBYTES = 32


def find_library():
    lib_name = "libmlwe-fast.so"
    if sys.platform == "win32":
        lib_name = "mlwe-fast.dll"
    elif sys.platform == "darwin":
        lib_name = "libmlwe-fast.dylib"

    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(script_dir)

    search_paths = [
        os.path.join(script_dir, lib_name),
        os.path.join(project_root, lib_name),
        os.path.join(project_root, "build", "crypto", lib_name),
        os.path.join(project_root, "build", "crypto", "Release", lib_name),
        os.path.join(project_root, "build", "crypto", "Debug", lib_name),
        os.path.join(project_root, "out", "build", "crypto", lib_name),
    ]

    for path in search_paths:
        if os.path.exists(path):
            return path

    return lib_name


def encrypt_decrypt_hybrid(data: bytes, key: bytes) -> bytes:
    out = bytearray()
    counter = 0
    for i in range(0, len(data), 32):
        block = data[i : i + 32]
        keystream = hashlib.sha256(key + counter.to_bytes(4, "big")).digest()
        for b, k in zip(block, keystream):
            out.append(b ^ k)
        counter += 1
    return bytes(out)


def main():
    lib_path = find_library()
    print(f"[*] Loading shared library from: {lib_path}")
    try:
        lib = ctypes.CDLL(lib_path)
    except OSError as e:
        print(f"[!] Error: Could not load the shared library: {e}")
        print("    Please build the project first (e.g., run cmake and make).")
        sys.exit(1)

    lib.mlwe_fast_version.restype = ctypes.c_char_p
    lib.mlwe_fast_version.argtypes = []

    lib.mlwe_fast_kem_keypair.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
    lib.mlwe_fast_kem_encaps.argtypes = [
        ctypes.c_void_p,
        ctypes.c_void_p,
        ctypes.c_void_p,
    ]
    lib.mlwe_fast_kem_decaps.argtypes = [
        ctypes.c_void_p,
        ctypes.c_void_p,
        ctypes.c_void_p,
    ]

    lib.mlwe_fast_kem_indcpa_keypair.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
    lib.mlwe_fast_kem_indcpa_enc.argtypes = [
        ctypes.c_void_p,
        ctypes.c_void_p,
        ctypes.c_void_p,
        ctypes.c_void_p,
    ]
    lib.mlwe_fast_kem_indcpa_dec.argtypes = [
        ctypes.c_void_p,
        ctypes.c_void_p,
        ctypes.c_void_p,
    ]

    version = lib.mlwe_fast_version().decode("utf-8")
    print(f"[*] MLWE-Fast library version: {version}")

    print(" 1. CRYSTALS-Kyber KEM Operations (IND-CCA2 Secure)")

    pk = ctypes.create_string_buffer(MLWE_FAST_PUBLICKEYBYTES)
    sk = ctypes.create_string_buffer(MLWE_FAST_SECRETKEYBYTES)
    c = ctypes.create_string_buffer(MLWE_FAST_CIPHERTEXTBYTES)
    ss_enc = ctypes.create_string_buffer(MLWE_FAST_SSBYTES)
    ss_dec = ctypes.create_string_buffer(MLWE_FAST_SSBYTES)

    print("[*] Generating KEM Keypair...")
    lib.mlwe_fast_kem_keypair(pk, sk)
    print(f"    - Public Key (first 16 bytes):  {pk.raw[:16].hex()}...")
    print(f"    - Secret Key (first 16 bytes):  {sk.raw[:16].hex()}...")

    print("[*] Encapsulating shared secret...")
    lib.mlwe_fast_kem_encaps(c, ss_enc, pk)
    print(f"    - Ciphertext (first 16 bytes):  {c.raw[:16].hex()}...")
    print(f"    - Encapsulated Shared Secret:  {ss_enc.raw.hex()}")

    print("[*] Decapsulating shared secret...")
    lib.mlwe_fast_kem_decaps(ss_dec, c, sk)
    print(f"    - Decapsulated Shared Secret:  {ss_dec.raw.hex()}")

    if ss_enc.raw == ss_dec.raw:
        print("[+] KEM Verification: SUCCESS (Shared secrets match!)")
    else:
        print("[-] KEM Verification: FAILURE (Shared secrets do not match!)")
        sys.exit(1)

    #

    print(" 2. Post-Quantum Hybrid Encryption of 'sample_input.txt' ")

    script_dir = os.path.dirname(os.path.abspath(__file__))
    input_path = os.path.join(os.path.dirname(script_dir), "sample_input.txt")
    if not os.path.exists(input_path):
        input_path = "sample_input.txt"

    try:
        with open(input_path, "rb") as f:
            plaintext = f.read()
    except IOError:
        plaintext = b"Fallback plaintext message. Hello, post-quantum world!"

    print(f"[*] Input File Path: {input_path}")
    print(f"[*] Plaintext length: {len(plaintext)} bytes")
    print(
        f"[*] Original Plaintext Preview:\n{plaintext.decode('utf-8', errors='replace')}"
    )

    shared_key = ss_enc.raw
    ciphertext_data = encrypt_decrypt_hybrid(plaintext, shared_key)
    print(f"[+] Encrypted ciphertext length: {len(ciphertext_data)} bytes")
    print(f"    - Ciphertext (first 32 bytes): {ciphertext_data[:32].hex()}...")

    decrypted_key = ss_dec.raw
    decrypted_data = encrypt_decrypt_hybrid(ciphertext_data, decrypted_key)
    print(f"[+] Decrypted text length: {len(decrypted_data)} bytes")
    print(f"[*] Decrypted Text Match: {decrypted_data == plaintext}")
    print(
        f"[*] Decrypted Text Preview:\n{decrypted_data.decode('utf-8', errors='replace')}"
    )

    print(" 3. Raw Passively Secure IND-CPA PKE wrappers")

    cpa_pk = ctypes.create_string_buffer(MLWE_FAST_PUBLICKEYBYTES)
    cpa_sk = ctypes.create_string_buffer(MLWE_FAST_INDCPA_SECRETKEYBYTES)
    cpa_c = ctypes.create_string_buffer(MLWE_FAST_CIPHERTEXTBYTES)

    message = bytearray(secrets.token_bytes(32))
    print(f"[*] Message to encrypt (32 bytes): {message.hex()}")

    coins = bytearray(secrets.token_bytes(32))
    print(f"[*] Random coins (32 bytes):       {coins.hex()}")

    print("[*] Generating IND-CPA Keypair...")
    lib.mlwe_fast_kem_indcpa_keypair(cpa_pk, cpa_sk)
    print(f"    - CPA Public Key (first 16 bytes):  {cpa_pk.raw[:16].hex()}...")
    print(f"    - CPA Secret Key (first 16 bytes):  {cpa_sk.raw[:16].hex()}...")

    print("[*] Encrypting message using IND-CPA...")
    msg_buf = ctypes.create_string_buffer(bytes(message), 32)
    coins_buf = ctypes.create_string_buffer(bytes(coins), 32)
    lib.mlwe_fast_kem_indcpa_enc(cpa_c, msg_buf, cpa_pk, coins_buf)
    print(f"    - CPA Ciphertext (first 16 bytes):  {cpa_c.raw[:16].hex()}...")

    dec_msg = ctypes.create_string_buffer(32)
    print("[*] Decrypting ciphertext using IND-CPA...")
    lib.mlwe_fast_kem_indcpa_dec(dec_msg, cpa_c, cpa_sk)
    print(f"    - CPA Decrypted Message:            {dec_msg.raw.hex()}")

    if dec_msg.raw == message:
        print("[+] IND-CPA Verification: SUCCESS (Decrypted message matches!)")
    else:
        print("[-] IND-CPA Verification: FAILURE (Decrypted message does not match!)")
        sys.exit(1)


if __name__ == "__main__":
    main()
