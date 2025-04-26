#include "include/args.h"
#include "include/logger.h"
#include "include/compress.h"
#include "include/file_io.h"
#include "include/tar.h"
#include "include/crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <libgen.h> 


int main(int argc, char *argv[]) {
    Arguments args = {0};
    init_logger("comp-tool.log");

    if (!parse_args(argc, argv, &args)) {
        print_usage(argv[0]);
        close_logger();
        return 1;
    }

    if (strcmp(args.operation, "compress") == 0 || strcmp(args.operation, "decompress") == 0) {
        if (strlen(args.password) == 0) {
            printf("Enter password (leave empty to skip): ");
            if (fgets(args.password, MAX_PASSWORD_LEN, stdin) == NULL) {
                fprintf(stderr, "Error reading password\n");
                close_logger();
                return 1;
            }
            args.password[strcspn(args.password, "\n")] = '\0';
        }
    }

    if (args.verbose) {
        log_info("Operation: %s\n", args.operation);
        log_info("Format: %s\n", args.format);
        log_info("Input: %s\n", args.input);
        log_info("Output: %s\n", args.output);
    }

    int result = 0;
    char *decrypted_temp = NULL;

    if (strcmp(args.operation, "compress") == 0) {
        if (strcmp(args.format, "zip") == 0) {
            result = zip_compress(args.input, args.output);
        } else if (strcmp(args.format, "huffman") == 0) {
            result = huffman_compress(args.input, args.output);
        } else if (strcmp(args.format, "rle") == 0) {
            result = rle_compress(args.input, args.output);
        } else {
            log_error("Unsupported format: %s\n", args.format);
            result = 1;
        }

        if (strlen(args.password) > 0 && result == 0) {
            size_t enc_len = strlen(args.output) + 5;
            char *encrypted_output = malloc(enc_len);
            if (!encrypted_output) {
                log_error("Memory allocation failed\n");
                close_logger();
                return 1;
            }
            snprintf(encrypted_output, enc_len, "%s.enc", args.output);

            result = encrypt_file(args.output, encrypted_output, args.password);
            if (result == 0) {
                remove(args.output);
                rename(encrypted_output, args.output);
            }
            free(encrypted_output);
        }

    } else if (strcmp(args.operation, "decompress") == 0) {
        char *temp_file = args.input;

        if (strlen(args.password) > 0) {
            char tmp_template[] = "/tmp/decrypted_XXXXXX";
            int fd = mkstemp(tmp_template);
            if (fd == -1) {
                perror("mkstemp failed");
                close_logger();
                return 1;
            }
            close(fd);

            decrypted_temp = strdup(tmp_template);
            if (!decrypted_temp) {
                log_error("Memory allocation failed\n");
                close_logger();
                return 1;
            }

            if (decrypt_file(args.input, decrypted_temp, args.password) != 0) {
                log_error("Failed to decrypt file with given password.\n");
                remove(decrypted_temp);
                free(decrypted_temp);
                close_logger();
                return 1;
            }

            temp_file = decrypted_temp;
        }

        if (strcmp(args.format, "zip") == 0) {
            // Nếu không có output, tạo thư mục với tên file zip
            if (strlen(args.output) == 0) {
                char *folder_name = strrchr(args.input, '/');
                if (!folder_name) folder_name = args.input;
                else folder_name++; // Bỏ đi dấu '/' phía trước tên file

                char output_folder[256];
                snprintf(output_folder, sizeof(output_folder), "%s", folder_name);

                if (mkdir(output_folder, 0777) == -1) {
                    log_error("Failed to create folder %s\n", output_folder);
                    return 1;
                }

                result = zip_decompress(temp_file, output_folder);
            } else {
                result = zip_decompress(temp_file, args.output);
            }
        } else if (strcmp(args.format, "huffman") == 0) {
            result = huffman_decompress(temp_file, args.output);
        } else if (strcmp(args.format, "rle") == 0) {
            result = rle_decompress(temp_file, args.output);
        } else {
            log_error("Unsupported format: %s\n", args.format);
            result = 1;
        }

        if (decrypted_temp != NULL) {
            remove(decrypted_temp);
            free(decrypted_temp);
        }

    } else if (strcmp(args.operation, "pack") == 0 && strcmp(args.format, "tar") == 0) {
        result = tar_pack(args.input, args.output);
    } else if (strcmp(args.operation, "unpack") == 0 && strcmp(args.format, "tar") == 0) {
        result = tar_unpack(args.input, args.output);
    } else if (strcmp(args.operation, "encrypt") == 0) {
        result = encrypt_file(args.input, args.output, "my-secret-key");
    } else if (strcmp(args.operation, "decrypt") == 0) {
        result = decrypt_file(args.input, args.output, "my-secret-key");
    } else {
        log_error("Unknown operation or format: %s, %s\n", args.operation, args.format);
        result = 1;
    }

    close_logger();
    return result;
}
