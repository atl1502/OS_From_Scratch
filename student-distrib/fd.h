#ifndef FILEDESC
#define FILEDESC

typedef struct fd {
	int32_t (*read) (uint32_t fd, void* buf, int32_t nbytes);
	int32_t (*write) (uint32_t fds, const void* buf, int32_t nbytes);
	int32_t (*close) (uint32_t fd);	int32_t inode_num;

	int32_t file_position;

	// most significant bit indicates active if 1
	int32_t flags;
} fd_t;



#endif
