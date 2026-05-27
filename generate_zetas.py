def bit_reverse(i, n):
    return int(format(i, f"0{n}b")[::-1], 2)


def generate_kyber_zetas():
    q = 3329
    n = 256
    zeta = 17
    R = 65536

    zetas = [0] * 128

    for i in range(128):
        power = pow(zeta, bit_reverse(i, 7), q)

        mont_zeta = (power * R) % q

        if mont_zeta > q // 2:
            mont_zeta -= q

        zetas[i] = mont_zeta

    return zetas


if __name__ == "__main__":
    zetas = generate_kyber_zetas()
    print("const int16_t mlwe_fast_zetas[128] = {")
    for i in range(0, 128, 8):
        line = ", ".join(f"{z:5}" for z in zetas[i : i + 8])
        print(f"    {line}" + ("," if i < 120 else ""))
    print("};")
