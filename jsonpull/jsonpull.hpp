#ifndef JSONPULL_HPP
#define JSONPULL_HPP

#include <string>
#include <vector>
#include <memory>

typedef enum json_type {
	// These types can be returned by json_read()
	JSON_HASH,
	JSON_ARRAY,
	JSON_NUMBER,
	JSON_STRING,
	JSON_TRUE,
	JSON_FALSE,
	JSON_NULL,

	// These and JSON_HASH and JSON_ARRAY can be called back by json_read_with_separators()
	JSON_COMMA,
	JSON_COLON,

	// These are only used internally as expectations of what comes next
	JSON_ITEM,
	JSON_KEY,
	JSON_VALUE,
} json_type;

struct json_object;

struct json_object {
	json_type type;
	std::shared_ptr<struct json_object> parent;
	struct json_pull *parser;

	std::string string;
	double number;

	std::vector<std::shared_ptr<json_object>> array;
	std::vector<std::shared_ptr<json_object>> keys;
	std::vector<std::shared_ptr<json_object>> values;

	int expect;
};

typedef struct json_pull {
	std::string error;
	size_t line;

	ssize_t (*read)(struct json_pull *, char *buf, size_t n);
	void *source;
	std::string buffer;
	ssize_t buffer_tail;
	ssize_t buffer_head;

	std::shared_ptr<json_object> container;
	std::shared_ptr<json_object> root;
} json_pull;

json_pull *json_begin_file(FILE *f);
json_pull *json_begin_string(const char *s);

json_pull *json_begin(ssize_t (*read)(struct json_pull *, char *buffer, size_t n), void *source);
void json_end(json_pull *p);

typedef void (*json_separator_callback)(json_type type, json_pull *j, void *state);

std::shared_ptr<json_object> json_read_tree(json_pull *j);
std::shared_ptr<json_object> json_read(json_pull *j);
std::shared_ptr<json_object> json_read_separators(json_pull *j, json_separator_callback cb, void *state);
void json_free(std::shared_ptr<json_object> j);
void json_disconnect(std::shared_ptr<json_object> j);

std::shared_ptr<json_object> json_hash_get(std::shared_ptr<json_object> o, std::string const &key);

std::string json_stringify(std::shared_ptr<json_object> o);

#endif
