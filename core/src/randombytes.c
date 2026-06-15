#include <lml/randombytes.h>

#ifdef _WIN32
#include <windows.h>
#include <bcrypt.h>

int randombytes(uint8_t *out, size_t outlen) {
    if (BCryptGenRandom(NULL, out, (ULONG)outlen, BCRYPT_USE_SYSTEM_PREFERRED_RNG) < 0) {
        return -1;
    }
    return 0;
}
#else
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int randombytes(uint8_t *out, size_t outlen) {
    static int fd = -1;
    ssize_t ret;

    if (fd == -1) {
        fd = open("/dev/urandom", O_RDONLY);
        if (fd == -1) return -1;
    }

    while (outlen > 0) {
        ret = read(fd, out, outlen);
        if (ret > 0) {
            out = out + ret;
            outlen = outlen - ret;
        } else if (ret == -1 && errno == EINTR) {
            continue;
        } else {
            return -1;
        }
    }
    return 0;
}
#endif
