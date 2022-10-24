#ifndef FILEDESC
#define FILEDESC

typedef struct fd {
	int32_t table_pointer;
	int32_t inode_num;
	int32_t file_position;
	int32_t flags;
} fd_t;

#endif
