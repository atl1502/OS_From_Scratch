#ifndef FILEDESC
#define FILEDESC

// get around circular definitions
typedef struct fd_ops fd_opts_t;

typedef struct fd {
	fd_opts_t* table_pointer;
	int32_t inode_num;
	int32_t file_position;

	// most significant bit indicates active if 1
	int32_t flags;
} fd_t;

struct fd_ops {
	int32_t (*read) (fd_t* fd, void* buf, int32_t nbytes);
	int32_t (*write) (fd_t* fd, const void* buf, int32_t nbytes);
	int32_t (*close) (fd_t* fd);
};

#endif
