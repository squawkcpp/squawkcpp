#ifndef BOOKRESULTITEM_H
#define BOOKRESULTITEM_H

namespace utils {
struct BookResultItem {
    int status = 200;
    int count = 0;
    std::string title, description, isbn, coverUri, author, publicationDate, publisher;
};
}//namespace utils
#endif // BOOKRESULTITEM_H
