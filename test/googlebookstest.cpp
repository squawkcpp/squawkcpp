/*
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <string>
#include <map>

#include "re2/re2.h"

#include "../src/utils/googlebooks.h"

#include <gtest/gtest.h>


TEST( GoogleBooksTest, TestParseQuery ) {
    std::string _query_response =
        R"json(
        {
         "items": [
          {
           "id": "LBd6b2n7MngC",
           "volumeInfo": {
            "title": "Weber's Grillbibel",
            "industryIdentifiers": [
             {
              "type": "ISBN_13",
              "identifier": "9783833830044"
             },
             {
              "type": "ISBN_10",
              "identifier": "3833830042"
             }
            ]
           }
          }
         ]
        }
        )json";

    EXPECT_EQ( "LBd6b2n7MngC", utils::GoogleBooks::parse_query( "", "9783833830044", _query_response ) );
    EXPECT_EQ( "", utils::GoogleBooks::parse_query( "", "1234", _query_response ) );
}

TEST( GoogleBooksTest, TestParseQueryTitle ) {
    std::string _query_response =
        R"json(
        {
         "items": [
          {
           "id": "LBd6b2n7MngC",
           "volumeInfo": {
            "title": "Weber's Grillbibel",
            "industryIdentifiers": [
             {
              "type": "ISBN_13",
              "identifier": "9783833830044"
             },
             {
              "type": "ISBN_10",
              "identifier": "3833830042"
             }
            ]
           }
          }
         ]
        }
        )json";

    EXPECT_EQ( "LBd6b2n7MngC", utils::GoogleBooks::parse_query( "(ABC) Weber's Grillbibel", "", _query_response ) );
    EXPECT_EQ( "LBd6b2n7MngC", utils::GoogleBooks::parse_query( "[ABC] Weber's Grillbibel", "", _query_response ) );
}

TEST( GoogleBooksTest, TestParseVolumes ) {
    std::string _query_response =
        R"json(
        {
         "kind": "books#volume",
         "id": "LBd6b2n7MngC",
         "etag": "49mJ1K7H/uc",
         "selfLink": "https://www.googleapis.com/books/v1/volumes/LBd6b2n7MngC",
         "volumeInfo": {
          "title": "Weber's Grillbibel",
          "authors": [
           "Jamie Purviance"
          ],
          "publisher": "Gräfe Und Unzer",
          "publishedDate": "2012-05-02",
          "description": "Das Buch zum perfekten Grillen! Der US-amerikanischen Grill-Guru Jamie Purviance verwöhnt uns mit 160 neuen Rezepten zum Grillen mit Gas und Holzkohle, deren einzelne Grundzubereitungen in mehr als 1000 Stepbildern Schritt für Schritt dargestellt werden: Rind-, Lamm- und Schweinefleisch, Fisch, Gemüse und Früchte - vom Grill. Grundtechniken rund ums Grillen, z.B. Zerteilen von Fleisch, Vorbereiten von Fisch, Gargradbestimmung, Saucen- und Pastenzubereitung, werden in Text und vielen Bildern genau beschrieben und lassen Grillen zu einem kulinarischen Hochgenuss werden. Sonderseiten mit Know-how zum sicheren Gelingen. Tabellen zum Nachschlagen, welches Stück wie lange auf den Grill muss und Rezepte für Saucen und Gewürzmischungen und zu welchem Fleisch sie passen, sind im Schlussteil des Buches zusammengefasst und sorgen dafür, dass dem Grillmeister nichts anbrennt.",
          "industryIdentifiers": [
           {
            "type": "ISBN_10",
            "identifier": "3833830042"
           },
           {
            "type": "ISBN_13",
            "identifier": "9783833830044"
           }
          ],
          "readingModes": {
           "text": true,
           "image": true
          },
          "pageCount": 320,
          "printedPageCount": 1051,
          "printType": "BOOK",
          "categories": [
           "Cooking / Courses & Dishes / General"
          ],
          "averageRating": 5.0,
          "ratingsCount": 2,
          "maturityRating": "NOT_MATURE",
          "allowAnonLogging": true,
          "contentVersion": "2.9.11.0.preview.3",
          "panelizationSummary": {
           "containsEpubBubbles": false,
           "containsImageBubbles": false
          },
          "imageLinks": {
           "smallThumbnail": "http://books.google.com/books/content?id=LBd6b2n7MngC&printsec=frontcover&img=1&zoom=5&edge=curl&imgtk=AFLRE73c1y6QuWXxF8nOmnGBX7tNDTTNduY36y_HIa1Kbt4qMozkt0C5xIBZHBTPhGPe-vjfWQSger63M0gymewlLP_EI5uHLyC9NCdCufwr5tSKGbhkBFuiTwA_BDF6OOkFPBgegB4y&source=gbs_api",
           "thumbnail": "http://books.google.com/books/content?id=LBd6b2n7MngC&printsec=frontcover&img=1&zoom=1&edge=curl&imgtk=AFLRE73XgDFsOz_CRPCSps6o5ZC4iMKpxL7IFyW3sz0EfwEdsrZ5KuAqkaTwzKMQpw4xxmWLjfXSvsp4v9aHevLwTAivnHkrHCK1SF8CpdgFgpA0xUZcZlm_9tmH13r7AIe9XVGdj-3K&source=gbs_api",
           "small": "http://books.google.com/books/content?id=LBd6b2n7MngC&printsec=frontcover&img=1&zoom=2&edge=curl&imgtk=AFLRE70mc2u6YGul-oi8M2vjG_WMarzEHPowwGyDK0v7wQpEkNuFRpMKX3lE_O8R35O8nx6n_-VjN9t1r15Qb7_rB4HjfGhA6zEncySVGKuGLr-VXXLdk465ECKdQ8rR2WP1jhBmb_Go&source=gbs_api",
           "medium": "http://books.google.com/books/content?id=LBd6b2n7MngC&printsec=frontcover&img=1&zoom=3&edge=curl&imgtk=AFLRE72zInJB-CL9-fXYg90iDl6-Xqyv_1HxSUejTHh04iAeJiPnfWfTutxkL5zGK2BYbfoG1LkGLqYQ4EuLtMJ1Zauhbaswdshr1Mk9Nu2_WIAmiPYKd-UI6XtC-Zc8UZ9_Ng46IfB5&source=gbs_api",
           "large": "http://books.google.com/books/content?id=LBd6b2n7MngC&printsec=frontcover&img=1&zoom=4&edge=curl&imgtk=AFLRE73MpgqKRu98AC0tIznZZ5buRRTG76X3fM5YUHkzWOaYVQgJ_zlZj9dncpCKOuwI79It6pWTrsSmUUNT8OynyK3sX3QwZK5ZwgSxW8OQisOq8GgrSeEK7f6ZB4Ot4Ecxgd6WBhzZ&source=gbs_api",
           "extraLarge": "http://books.google.com/books/content?id=LBd6b2n7MngC&printsec=frontcover&img=1&zoom=6&edge=curl&imgtk=AFLRE72lyYF5DnqPowIpbdpWNvesavMS0JYTkYXk4J3vtjn8dRYb1H-4MKL47DhcQ1L0sgE6kIC9GpnPOvitehQ68jmlt955MmATMnQ4JnBqGQwYpn4c1_BZ5FpUMkKrqoxPaa9GCDQu&source=gbs_api"
          },
          "language": "de",
          "previewLink": "http://books.google.ch/books?id=LBd6b2n7MngC&hl=&source=gbs_api",
          "infoLink": "https://play.google.com/store/books/details?id=LBd6b2n7MngC&source=gbs_api",
          "canonicalVolumeLink": "https://market.android.com/details?id=book-LBd6b2n7MngC"
         },
         "layerInfo": {
          "layers": [
           {
            "layerId": "geo",
            "volumeAnnotationsVersion": "15"
           }
          ]
         },
         "saleInfo": {
          "country": "CH",
          "saleability": "FOR_SALE",
          "isEbook": true,
          "listPrice": {
           "amount": 30.0,
           "currencyCode": "CHF"
          },
          "retailPrice": {
           "amount": 21.0,
           "currencyCode": "CHF"
          },
          "buyLink": "https://play.google.com/store/books/details?id=LBd6b2n7MngC&rdid=book-LBd6b2n7MngC&rdot=1&source=gbs_api",
          "offers": [
           {
            "finskyOfferType": 1,
            "listPrice": {
             "amountInMicros": 3.0E7,
             "currencyCode": "CHF"
            },
            "retailPrice": {
             "amountInMicros": 2.1E7,
             "currencyCode": "CHF"
            },
            "giftable": true
           }
          ]
         },
         "accessInfo": {
          "country": "CH",
          "viewability": "PARTIAL",
          "embeddable": true,
          "publicDomain": false,
          "textToSpeechPermission": "ALLOWED",
          "epub": {
           "isAvailable": true,
           "acsTokenLink": "http://books.google.ch/books/download/Weber_s_Grillbibel-sample-epub.acsm?id=LBd6b2n7MngC&format=epub&output=acs4_fulfillment_token&dl_type=sample&source=gbs_api"
          },
          "pdf": {
           "isAvailable": true,
           "acsTokenLink": "http://books.google.ch/books/download/Weber_s_Grillbibel-sample-pdf.acsm?id=LBd6b2n7MngC&format=pdf&output=acs4_fulfillment_token&dl_type=sample&source=gbs_api"
          },
          "webReaderLink": "http://books.google.ch/books/reader?id=LBd6b2n7MngC&hl=&printsec=frontcover&output=reader&source=gbs_api",
          "accessViewStatus": "SAMPLE",
          "quoteSharingAllowed": false
         }
        }
        )json";
    utils::BookResultItem _item = utils::GoogleBooks::parse_volumes( _query_response );
    EXPECT_EQ( "Jamie Purviance", _item.author );
    EXPECT_EQ( "http://books.google.com/books/content?id=LBd6b2n7MngC&printsec=frontcover&img=1&zoom=4&edge=curl&imgtk=AFLRE73MpgqKRu98AC0tIznZZ5buRRTG76X3fM5YUHkzWOaYVQgJ_zlZj9dncpCKOuwI79It6pWTrsSmUUNT8OynyK3sX3QwZK5ZwgSxW8OQisOq8GgrSeEK7f6ZB4Ot4Ecxgd6WBhzZ&source=gbs_api", _item.coverUri);
    EXPECT_EQ( "Das Buch zum perfekten Grillen! Der US-amerikanischen Grill-Guru Jamie Purviance verwöhnt uns mit 160 neuen Rezepten zum Grillen mit Gas und Holzkohle, deren einzelne Grundzubereitungen in mehr als 1000 Stepbildern Schritt für Schritt dargestellt werden: Rind-, Lamm- und Schweinefleisch, Fisch, Gemüse und Früchte - vom Grill. Grundtechniken rund ums Grillen, z.B. Zerteilen von Fleisch, Vorbereiten von Fisch, Gargradbestimmung, Saucen- und Pastenzubereitung, werden in Text und vielen Bildern genau beschrieben und lassen Grillen zu einem kulinarischen Hochgenuss werden. Sonderseiten mit Know-how zum sicheren Gelingen. Tabellen zum Nachschlagen, welches Stück wie lange auf den Grill muss und Rezepte für Saucen und Gewürzmischungen und zu welchem Fleisch sie passen, sind im Schlussteil des Buches zusammengefasst und sorgen dafür, dass dem Grillmeister nichts anbrennt.", _item.description );
    EXPECT_EQ( "3833830042", _item.isbn);
    EXPECT_EQ( "2012-05-02", _item.publicationDate );
    EXPECT_EQ( "Gräfe Und Unzer", _item.publisher );
    EXPECT_EQ( "Weber's Grillbibel", _item.title );
}

TEST( GoogleBooksTest, TestParseCoverFallback ) {
    std::string _query_response =
        R"json(
        {
         "kind": "books#volume",
         "id": "LBd6b2n7MngC",
         "etag": "49mJ1K7H/uc",
         "selfLink": "https://www.googleapis.com/books/v1/volumes/LBd6b2n7MngC",
         "volumeInfo": {
          "title": "Weber's Grillbibel",
          "authors": [
           "Jamie Purviance"
          ],
          "publisher": "Gräfe Und Unzer",
          "publishedDate": "2012-05-02",
          "description": "Das Buch zum perfekten Grillen! Der US-amerikanischen Grill-Guru Jamie Purviance verwöhnt uns mit 160 neuen Rezepten zum Grillen mit Gas und Holzkohle, deren einzelne Grundzubereitungen in mehr als 1000 Stepbildern Schritt für Schritt dargestellt werden: Rind-, Lamm- und Schweinefleisch, Fisch, Gemüse und Früchte - vom Grill. Grundtechniken rund ums Grillen, z.B. Zerteilen von Fleisch, Vorbereiten von Fisch, Gargradbestimmung, Saucen- und Pastenzubereitung, werden in Text und vielen Bildern genau beschrieben und lassen Grillen zu einem kulinarischen Hochgenuss werden. Sonderseiten mit Know-how zum sicheren Gelingen. Tabellen zum Nachschlagen, welches Stück wie lange auf den Grill muss und Rezepte für Saucen und Gewürzmischungen und zu welchem Fleisch sie passen, sind im Schlussteil des Buches zusammengefasst und sorgen dafür, dass dem Grillmeister nichts anbrennt.",
          "industryIdentifiers": [
           {
            "type": "ISBN_10",
            "identifier": "3833830042"
           },
           {
            "type": "ISBN_13",
            "identifier": "9783833830044"
           }
          ],
          "readingModes": {
           "text": true,
           "image": true
          },
          "pageCount": 320,
          "printedPageCount": 1051,
          "printType": "BOOK",
          "categories": [
           "Cooking / Courses & Dishes / General"
          ],
          "averageRating": 5.0,
          "ratingsCount": 2,
          "maturityRating": "NOT_MATURE",
          "allowAnonLogging": true,
          "contentVersion": "2.9.11.0.preview.3",
          "panelizationSummary": {
           "containsEpubBubbles": false,
           "containsImageBubbles": false
          },
          "imageLinks": {
           "smallThumbnail": "http://books.google.com/books/content?id=LBd6b2n7MngC&printsec=frontcover&img=1&zoom=5&edge=curl&imgtk=AFLRE73c1y6QuWXxF8nOmnGBX7tNDTTNduY36y_HIa1Kbt4qMozkt0C5xIBZHBTPhGPe-vjfWQSger63M0gymewlLP_EI5uHLyC9NCdCufwr5tSKGbhkBFuiTwA_BDF6OOkFPBgegB4y&source=gbs_api",
           "thumbnail": "http://books.google.com/books/content?id=LBd6b2n7MngC&printsec=frontcover&img=1&zoom=1&edge=curl&imgtk=AFLRE73XgDFsOz_CRPCSps6o5ZC4iMKpxL7IFyW3sz0EfwEdsrZ5KuAqkaTwzKMQpw4xxmWLjfXSvsp4v9aHevLwTAivnHkrHCK1SF8CpdgFgpA0xUZcZlm_9tmH13r7AIe9XVGdj-3K&source=gbs_api",
           "small": "http://books.google.com/books/content?id=LBd6b2n7MngC&printsec=frontcover&img=1&zoom=2&edge=curl&imgtk=AFLRE70mc2u6YGul-oi8M2vjG_WMarzEHPowwGyDK0v7wQpEkNuFRpMKX3lE_O8R35O8nx6n_-VjN9t1r15Qb7_rB4HjfGhA6zEncySVGKuGLr-VXXLdk465ECKdQ8rR2WP1jhBmb_Go&source=gbs_api",
           "medium": "http://books.google.com/books/content?id=LBd6b2n7MngC&printsec=frontcover&img=1&zoom=3&edge=curl&imgtk=AFLRE72zInJB-CL9-fXYg90iDl6-Xqyv_1HxSUejTHh04iAeJiPnfWfTutxkL5zGK2BYbfoG1LkGLqYQ4EuLtMJ1Zauhbaswdshr1Mk9Nu2_WIAmiPYKd-UI6XtC-Zc8UZ9_Ng46IfB5&source=gbs_api"
          },
          "language": "de",
          "previewLink": "http://books.google.ch/books?id=LBd6b2n7MngC&hl=&source=gbs_api",
          "infoLink": "https://play.google.com/store/books/details?id=LBd6b2n7MngC&source=gbs_api",
          "canonicalVolumeLink": "https://market.android.com/details?id=book-LBd6b2n7MngC"
         },
         "layerInfo": {
          "layers": [
           {
            "layerId": "geo",
            "volumeAnnotationsVersion": "15"
           }
          ]
         },
         "saleInfo": {
          "country": "CH",
          "saleability": "FOR_SALE",
          "isEbook": true,
          "listPrice": {
           "amount": 30.0,
           "currencyCode": "CHF"
          },
          "retailPrice": {
           "amount": 21.0,
           "currencyCode": "CHF"
          },
          "buyLink": "https://play.google.com/store/books/details?id=LBd6b2n7MngC&rdid=book-LBd6b2n7MngC&rdot=1&source=gbs_api",
          "offers": [
           {
            "finskyOfferType": 1,
            "listPrice": {
             "amountInMicros": 3.0E7,
             "currencyCode": "CHF"
            },
            "retailPrice": {
             "amountInMicros": 2.1E7,
             "currencyCode": "CHF"
            },
            "giftable": true
           }
          ]
         },
         "accessInfo": {
          "country": "CH",
          "viewability": "PARTIAL",
          "embeddable": true,
          "publicDomain": false,
          "textToSpeechPermission": "ALLOWED",
          "epub": {
           "isAvailable": true,
           "acsTokenLink": "http://books.google.ch/books/download/Weber_s_Grillbibel-sample-epub.acsm?id=LBd6b2n7MngC&format=epub&output=acs4_fulfillment_token&dl_type=sample&source=gbs_api"
          },
          "pdf": {
           "isAvailable": true,
           "acsTokenLink": "http://books.google.ch/books/download/Weber_s_Grillbibel-sample-pdf.acsm?id=LBd6b2n7MngC&format=pdf&output=acs4_fulfillment_token&dl_type=sample&source=gbs_api"
          },
          "webReaderLink": "http://books.google.ch/books/reader?id=LBd6b2n7MngC&hl=&printsec=frontcover&output=reader&source=gbs_api",
          "accessViewStatus": "SAMPLE",
          "quoteSharingAllowed": false
         }
        }
        )json";
    utils::BookResultItem _item = utils::GoogleBooks::parse_volumes( _query_response );
    EXPECT_EQ( "Jamie Purviance", _item.author );
    EXPECT_EQ( "http://books.google.com/books/content?id=LBd6b2n7MngC&printsec=frontcover&img=1&zoom=3&edge=curl&imgtk=AFLRE72zInJB-CL9-fXYg90iDl6-Xqyv_1HxSUejTHh04iAeJiPnfWfTutxkL5zGK2BYbfoG1LkGLqYQ4EuLtMJ1Zauhbaswdshr1Mk9Nu2_WIAmiPYKd-UI6XtC-Zc8UZ9_Ng46IfB5&source=gbs_api", _item.coverUri);
    EXPECT_EQ( "Das Buch zum perfekten Grillen! Der US-amerikanischen Grill-Guru Jamie Purviance verwöhnt uns mit 160 neuen Rezepten zum Grillen mit Gas und Holzkohle, deren einzelne Grundzubereitungen in mehr als 1000 Stepbildern Schritt für Schritt dargestellt werden: Rind-, Lamm- und Schweinefleisch, Fisch, Gemüse und Früchte - vom Grill. Grundtechniken rund ums Grillen, z.B. Zerteilen von Fleisch, Vorbereiten von Fisch, Gargradbestimmung, Saucen- und Pastenzubereitung, werden in Text und vielen Bildern genau beschrieben und lassen Grillen zu einem kulinarischen Hochgenuss werden. Sonderseiten mit Know-how zum sicheren Gelingen. Tabellen zum Nachschlagen, welches Stück wie lange auf den Grill muss und Rezepte für Saucen und Gewürzmischungen und zu welchem Fleisch sie passen, sind im Schlussteil des Buches zusammengefasst und sorgen dafür, dass dem Grillmeister nichts anbrennt.", _item.description );
    EXPECT_EQ( "3833830042", _item.isbn);
    EXPECT_EQ( "2012-05-02", _item.publicationDate );
    EXPECT_EQ( "Gräfe Und Unzer", _item.publisher );
    EXPECT_EQ( "Weber's Grillbibel", _item.title );
}
