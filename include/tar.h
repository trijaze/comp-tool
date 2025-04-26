#ifndef TAR_H
#define TAR_H

int tar_pack(const char *input_path, const char *output_path);
int tar_unpack(const char *input_path, const char *output_dir);

#endif
