#include "type_resolver.h"

Type* resolve_type(ASTNode* node) {
	Type* type = type_make_primitive(typekind_from_str(node->type.name.data));
	return type;
}
