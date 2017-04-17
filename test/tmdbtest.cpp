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

#include "../src/utils/tmdb.h"

#include <gtest/gtest.h>

namespace tmdb {

TEST( TmdbTest, parse_result ) {
    std::string _result = "{\"page\":1,\"results\":[{\"poster_path\":\"\/1yeVJox3rjo2jBKrrihIMj7uoS9.jpg\",\"popularity\":21.434502,\"id\":1396,\"backdrop_path\":\"\/eSzpy96DwBujGFj0xMbXBcGcfxX.jpg\",\"vote_average\":8.03,\"overview\":\"Breaking Bad is an American crime drama television series created and produced by Vince Gilligan. Set and produced in Albuquerque, New Mexico, Breaking Bad is the story of Walter White, a struggling high school chemistry teacher who is diagnosed with inoperable lung cancer at the beginning of the series. He turns to a life of crime, producing and selling methamphetamine, in order to secure his family's financial future before he dies, teaming with his former student, Jesse Pinkman. Heavily serialized, the series is known for positioning its characters in seemingly inextricable corners and has been labeled a contemporary western by its creator.\",\"first_air_date\":\"2008-01-19\",\"origin_country\":[\"US\"],\"genre_ids\":[18],\"original_language\":\"en\",\"vote_count\":944,\"name\":\"Breaking Bad\",\"original_name\":\"Breaking Bad\"}],\"total_results\":1,\"total_pages\":1}";
    std::string _comment = "Breaking Bad is an American crime drama television series created and produced by Vince Gilligan. Set and produced in Albuquerque, New Mexico, Breaking Bad is the story of Walter White, a struggling high school chemistry teacher who is diagnosed with inoperable lung cancer at the beginning of the series. He turns to a life of crime, producing and selling methamphetamine, in order to secure his family's financial future before he dies, teaming with his former student, Jesse Pinkman. Heavily serialized, the series is known for positioning its characters in seemingly inextricable corners and has been labeled a contemporary western by its creator.";
    struct tm _tm = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    strptime( "2008-01-19", "%Y-%m-%d", &_tm );
    time_t tmp_time_ = mktime ( &_tm );

    std::vector < Series > _series_list = Tmdb::parse_series( _result );
    EXPECT_EQ( 1U, _series_list.size() );
    Series _series = _series_list.front();
    EXPECT_EQ( "/eSzpy96DwBujGFj0xMbXBcGcfxX.jpg", _series.backdrop );
    EXPECT_EQ( _comment, _series.comment );
    EXPECT_EQ( tmp_time_, _series.first_air_date );
    EXPECT_EQ( "/1yeVJox3rjo2jBKrrihIMj7uoS9.jpg", _series.poster );
    EXPECT_EQ( "Breaking Bad", _series.title );
    EXPECT_EQ( 1396U, _series.tmdb_id );
}

TEST( TmdbTest, parse_result_gold_rush ) {

    std::string _gold_rush =
        R"json({"page":1,"results":[{"poster_path":"\/5gCWm452F6VhnRUaAjehuO8J3dP.jpg","popularity":11.206617,"id":34634,"backdrop_path":"\/zl3M2Ejo3vsvqLjRxz9VXBjER7R.jpg","vote_average":5.69,"overview":"Follow the lives of ambitious miners as they head north in pursuit of gold. With new miners, new claims, new machines and new ways to pull gold out of the ground, the stakes are higher than ever. But will big risks lead to an even bigger pay out?","first_air_date":"2010-12-03","origin_country":["US"],"genre_ids":[10764,99],"original_language":"en","vote_count":32,"name":"Gold Rush","original_name":"Gold Rush"},{"poster_path":null,"popularity":1.000477,"id":10220,"backdrop_path":null,"vote_average":0,"overview":"Gold Rush is a reality competition created by Mark Burnett and AOL and hosted by Mark Steines. The format is of an internet scavenger hunt that is offering chances to win $US50,000, $100,000 and $1,000,000.\n\nGold Rush consists of 13 rounds of game play. In order to qualify for a chance to win, participants will have to correctly complete a series of tasks on AOL.com\u2019s Gold Rush hub in order to stockpile virtual gold bars. Many of these tasks will consist of pop culture trivia challenges. Clues to help solve each of the challenges will be found in CBS Television programs and commercials, magazines, radio, song lyrics, and on AOL.\n\nIn each round, the first three players who complete the challenges and collect 12 virtual gold bars will be taken to a location somewhere in the United States where they will compete on-camera in a head-to-head, reality-style competition for a chance to win $100,000 in gold. In the Finale Round of Gold Rush, the 12 previous $100,000 winners return, joined by 6 new contestants, to vie for the $1 million grand prize.\n\nVarious companies have all signed on as partners of Gold Rush. Each company\u2019s brand will be integrated into various parts of Gold Rush, including game clues and challenges throughout the game.","first_air_date":"","origin_country":["US"],"genre_ids":[],"original_language":"en","vote_count":0,"name":"Gold Rush","original_name":"Gold Rush"},{"poster_path":"\/gJTJid8t6xBROJqq6A7BMCQuZpo.jpg","popularity":1.040859,"id":66032,"backdrop_path":"\/jZmILCAC9qi8TdnDPRhzN0H6zIB.jpg","vote_average":0,"overview":"","first_air_date":"2016-03-03","origin_country":[],"genre_ids":[],"original_language":"en","vote_count":0,"name":"Gold Rush: Legends","original_name":"Gold Rush: Legends"},{"poster_path":null,"popularity":1.008572,"id":64922,"backdrop_path":null,"vote_average":0,"overview":"","first_air_date":"2012-11-09","origin_country":[],"genre_ids":[],"original_language":"en","vote_count":0,"name":"Gold Rush: Dirt to Riches","original_name":"Gold Rush: Dirt to Riches"}],"total_results":4,"total_pages":1})json";

    std::vector < Series > _series_list = Tmdb::parse_series( _gold_rush );
    EXPECT_EQ( 4U, _series_list.size() );
    Series _found;
    for( auto _serie : _series_list ) {
        if( _serie.title == "Gold Rush" )
            _found = _serie;
    }
    EXPECT_EQ( "Gold Rush", _found.title );
}
}//namespace tmdb
