#include"document.h"
std::ostream& operator <<(std::ostream& d_out, const Document& doc)
{
    d_out << "{ document_id = " << doc.id << ", relevance = " << doc.relevance << ", rating = " << doc.rating << " }";
    return d_out;
}