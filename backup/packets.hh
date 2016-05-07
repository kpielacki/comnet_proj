
typedef enum {
	REQUEST = 0,
	RESPONSE,
	UPDATE
} msg_t;

typedef struct {
	u_char type;
	u_char content_id;
} request_header_t;

typedef struct {
	u_char type;
	u_char content_id;
} response_header_t;

typedef struct {
	u_char type;
	u_char content_id;
	
} update_header_t;
	
