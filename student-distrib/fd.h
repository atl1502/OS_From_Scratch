#ifndef FILEDESC
#define FILEDESC

typedef struct fd_ops {
	int32_t (*read) (int32_t fd, void* buf, int32_t nbytes);
	int32_t (*write) (int32_t fd, const void* buf, int32_t nbytes);
	int32_t (*open) (const uint8_t* filename, int32_t fd);
	int32_t (*close) (int32_t fd);
} fd_opts_t;

typedef struct fd {
	fd_opts_t* table_pointer;
	int32_t inode_num;
	int32_t file_position;

	// most significant bit indicates active if 1
	int32_t flags; 
} fd_t;

#endif
