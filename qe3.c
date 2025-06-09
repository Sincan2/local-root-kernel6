/*
 ============================================================================
 Name        : qe3_fixed.c
 Author      : Sincan2 MHL Team
 Description : Membuat user namespace baru DAN memetakan user saat ini
               ke root di dalam namespace tersebut.
 ============================================================================
*/

#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

void write_to_file(const char *path, const char *content) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        perror("Gagal membuka file map");
        exit(EXIT_FAILURE);
    }
    ssize_t written = write(fd, content, strlen(content));
    if (written < 0) {
        perror("Gagal menulis ke file map");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    uid_t real_uid = getuid();
    gid_t real_gid = getgid();
    char uid_map[128], gid_map[128];

    printf("BY Sincan2 MHL Team\n");

    // untuk menulis ke file map-nya sendiri.
    if (unshare(CLONE_NEWUSER) != 0) {
        perror("Gagal menjalankan unshare()");
        exit(EXIT_FAILURE);
    }

    printf("Ketik uname -a;id\n");

    // Kita harus menonaktifkan setgroups terlebih dahulu.
    write_to_file("/proc/self/setgroups", "deny");
    snprintf(gid_map, sizeof(gid_map), "0 %d 1", real_gid);
    write_to_file("/proc/self/gid_map", gid_map);

    // Menulis pemetaan UID.
    snprintf(uid_map, sizeof(uid_map), "0 %d 1", real_uid);
    write_to_file("/proc/self/uid_map", uid_map);

    printf("Pemetaan selesai. Menjalankan shell (/bin/sh).\n\n");

    // Menjalankan shell. Sekarang shell ini akan berjalan sebagai root di dalam namespace.
    if (execlp("/bin/sh", "sh", NULL) == -1) {
        perror("Gagal menjalankan execlp()");
        exit(EXIT_FAILURE);
    }

    // Kode di bawah ini tidak akan pernah dieksekusi kecuali execlp gagal
    return 0;
}
