///*
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.

//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.

//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//*/

//#include <string>
//#include <fstream>
//#include <map>

//#include "../src/utils/libav.h"
////#include "../src/utils/audio.h"

//#include <gtest/gtest.h>

//namespace libav {

//TEST( LibavTest, err_condition ) {
//    std::error_code errc = std::error_code( AVERROR_BSF_NOT_FOUND, av_category );
//    EXPECT_EQ( AV_BSF_NOT_FOUND, errc.value() );

//    errc = make_error_code( AVERROR_BSF_NOT_FOUND );
//    EXPECT_EQ( AV_BSF_NOT_FOUND, errc.value() );

//    errc = make_error_code( -2 );
//    EXPECT_EQ( ENOENT, errc.value() );
//    EXPECT_EQ( "No such file or directory", errc.message() );
//}

//TEST( LibavTest, errc ) {
//    std::string filename = "/home/e3a/testfiles/sample.flac";

//    Libav _audio;
//    std::error_code errc = _audio.input( filename.c_str() );
//    ASSERT_FALSE( errc );

//    errc = _audio.input( "dum_trala.mp3" );
//    ASSERT_EQ( ENOENT, errc.value() );
//    ASSERT_EQ( "No such file or directory", errc.message() );
//}

//TEST( LibavTest, open_audio_file_metadata_file ) {
//    std::string filename = std::string( TESTFILES ) + "BIS1536-001-flac_16-nocover.flac";

//    Libav _audio;
//    _audio.input( filename.c_str() );

//    EXPECT_EQ ( 1, _audio.nb_streams() );

//    MediaInfo _type = _audio.media_info( 0 );
//    EXPECT_EQ ( 1, _audio.nb_streams() );
//    EXPECT_EQ ( MediaType::AUDIO, _type.type );
//    EXPECT_EQ ( Codec::FLAC, _type.codec );
//    EXPECT_EQ ( 0 /*TODO*/, _type.bitrate );
//    EXPECT_EQ ( 44100, _type.samplerate );
//    EXPECT_EQ ( 2, _type.channels );
//    EXPECT_EQ ( 9078720, _type.playtime );
//    EXPECT_EQ ( 0, _type.bits_per_sample ); //TODO
//    EXPECT_EQ ( 0, _type.width );
//    EXPECT_EQ ( 0, _type.height );

//    EXPECT_EQ ( 11U, _type.metadata.tag_names().size() );
//    EXPECT_EQ ( "Victorious Love - Carolyn Sampson sings Purcell", _type.metadata.get( Metadata::ALBUM ) );
//    EXPECT_EQ ( "Sampson, Carolyn;Cummings, Laurence;Kenny, Elizabeth;Lasla, Anne-Marie", _type.metadata.get( Metadata::ARTIST ) );
//    EXPECT_EQ ( "Downloaded from eClassical.com. From album BIS-SACD-1536", _type.metadata.get( Metadata::COMMENT ) );
//    EXPECT_EQ ( "Purcell, Henry", _type.metadata.get( Metadata::COMPOSER ) );
//    EXPECT_EQ ( "1/1", _type.metadata.get( Metadata::DISC) );
//    EXPECT_EQ ( "(32)Vocal/Choral;(32)Baroque", _type.metadata.get( Metadata::GENRE ) );
//    EXPECT_EQ ( "Sampson, Carolyn;Cummings, Laurence;Kenny, Elizabeth;Lasla, Anne-Marie", _type.metadata.get( Metadata::PERFORMER ) );
//    EXPECT_EQ ( "BIS;eClassical", _type.metadata.get( Metadata::PUBLISHER ) );
//    EXPECT_EQ ( "(Sweeter than roses, Z.585 No.1) - Sweeter than roses, Z.585 No.1", _type.metadata.get( Metadata::TITLE ) );
//    EXPECT_EQ ( "1/19", _type.metadata.get( Metadata::TRACK ) );
//    EXPECT_EQ ( "2007-08-30", _type.metadata.get( Metadata::YEAR ) );
//}

//TEST( LibavTest, transcode_audio_file ) {
//    std::string filename = std::string( TESTFILES ) + "BIS1536-001-flac_16-nocover.flac";

//    { //Transcode
//        std::cout << "============================== TRANSCODE" << std::endl;
//        Libav _audio;
//        _audio.input( filename.c_str() );
//        _audio.output( "/tmp/file_out.mp3" );
//        MediaInfo _media_info = _audio.media_info( 0 );
//        _audio.transcode( _media_info );
//        std::cout << "============================== END TRANSCODE" << std::endl;
//    }

//    { //open and test transcoded file
//        std::cout << "============================== RE-OPEN" << std::endl;
//        Libav _audio;
//        _audio.input( "/tmp/file_out.mp3" );

//    EXPECT_EQ ( 1, _audio.nb_streams() );

//    MediaInfo _type = _audio.media_info( 0 );
//    EXPECT_EQ ( 1, _audio.nb_streams() );
//    EXPECT_EQ ( MediaType::AUDIO, _type.type );
//    EXPECT_EQ ( Codec::MP3, _type.codec );
//    EXPECT_EQ ( 96000 /*TODO*/, _type.bitrate );
//    EXPECT_EQ ( 44100, _type.samplerate );
//    EXPECT_EQ ( 2, _type.channels );
//    EXPECT_EQ ( 9078720, _type.playtime );
//    EXPECT_EQ ( 0, _type.bits_per_sample ); //TODO
//    EXPECT_EQ ( 0, _type.width );
//    EXPECT_EQ ( 0, _type.height );

//    EXPECT_EQ ( 11U, _type.metadata.tag_names().size() );
////    EXPECT_EQ ( "Victorious Love - Carolyn Sampson sings Purcell", _type.metadata.get( Metadata::ALBUM ) );
////    EXPECT_EQ ( "Sampson, Carolyn;Cummings, Laurence;Kenny, Elizabeth;Lasla, Anne-Marie", _type.metadata.get( Metadata::ARTIST ) );
////    EXPECT_EQ ( "Downloaded from eClassical.com. From album BIS-SACD-1536", _type.metadata.get( Metadata::COMMENT ) );
////    EXPECT_EQ ( "Purcell, Henry", _type.metadata.get( Metadata::COMPOSER ) );
////    EXPECT_EQ ( "1/1", _type.metadata.get( Metadata::DISC) );
////    EXPECT_EQ ( "(32)Vocal/Choral;(32)Baroque", _type.metadata.get( Metadata::GENRE ) );
////    EXPECT_EQ ( "Sampson, Carolyn;Cummings, Laurence;Kenny, Elizabeth;Lasla, Anne-Marie", _type.metadata.get( Metadata::PERFORMER ) );
////    EXPECT_EQ ( "BIS;eClassical", _type.metadata.get( Metadata::PUBLISHER ) );
////    EXPECT_EQ ( "(Sweeter than roses, Z.585 No.1) - Sweeter than roses, Z.585 No.1", _type.metadata.get( Metadata::TITLE ) );
////    EXPECT_EQ ( "1/19", _type.metadata.get( Metadata::TRACK ) );
////    EXPECT_EQ ( "2007-08-30", _type.metadata.get( Metadata::YEAR ) );

//    }
//}

////TEST( LibavTest, open_audio_file_metadata_stream ) {
////    std::string filename = std::string( TESTFILES ) + "BIS1536-001-flac_16-nocover.flac";

////    Audio _audio;
////    std::fstream _fs ( filename );
////    _audio.input( &_fs );

////    EXPECT_EQ ( 1, _audio.nb_streams() );

////    MediaInfo _type = _audio.media_info( 0 );
////    EXPECT_EQ ( 1, _audio.nb_streams() );
////    EXPECT_EQ ( MediaType::AUDIO, _type.type );
////    EXPECT_EQ ( Codec::FLAC, _type.codec );
////    EXPECT_EQ ( 0 /*TODO*/, _type.bitrate );
////    EXPECT_EQ ( 44100, _type.samplerate );
////    EXPECT_EQ ( 2, _type.channels );
////    EXPECT_EQ ( 9078720, _type.playtime );
////    EXPECT_EQ ( 0, _type.bits_per_sample ); //TODO
////    EXPECT_EQ ( 0, _type.width );
////    EXPECT_EQ ( 0, _type.height );

////    EXPECT_EQ ( 11U, _type.metadata.tag_names().size() );
////    EXPECT_EQ ( "Victorious Love - Carolyn Sampson sings Purcell", _type.metadata.get( Metadata::ALBUM ) );
////    EXPECT_EQ ( "Sampson, Carolyn;Cummings, Laurence;Kenny, Elizabeth;Lasla, Anne-Marie", _type.metadata.get( Metadata::ARTIST ) );
////    EXPECT_EQ ( "Downloaded from eClassical.com. From album BIS-SACD-1536", _type.metadata.get( Metadata::COMMENT ) );
////    EXPECT_EQ ( "Purcell, Henry", _type.metadata.get( Metadata::COMPOSER ) );
////    EXPECT_EQ ( "1/1", _type.metadata.get( Metadata::DISC) );
////    EXPECT_EQ ( "(32)Vocal/Choral;(32)Baroque", _type.metadata.get( Metadata::GENRE ) );
////    EXPECT_EQ ( "Sampson, Carolyn;Cummings, Laurence;Kenny, Elizabeth;Lasla, Anne-Marie", _type.metadata.get( Metadata::PERFORMER ) );
////    EXPECT_EQ ( "BIS;eClassical", _type.metadata.get( Metadata::PUBLISHER ) );
////    EXPECT_EQ ( "(Sweeter than roses, Z.585 No.1) - Sweeter than roses, Z.585 No.1", _type.metadata.get( Metadata::TITLE ) );
////    EXPECT_EQ ( "1/19", _type.metadata.get( Metadata::TRACK ) );
////    EXPECT_EQ ( "2007-08-30", _type.metadata.get( Metadata::YEAR ) );
////}

////TEST( LibavTest, transcode_file_stream ) {
////    { //transcode file
////        std::string filename = std::string( TESTFILES ) + "01 - I. Feierlich, misterioso.flac";
//////        std::string filename = std::string( TESTFILES ) + "BIS1536-001-flac_16-nocover.flac";
////        Audio _audio;
//////        std::fstream _fs ( filename );
////        _audio.input( filename );

////        EXPECT_EQ ( 1, _audio.nb_streams() );
////        MediaInfo _type = _audio.media_info( 0 );
////        _type.bitrate = 128000;
////        _type.codec = Codec::AAC;
////        _type.samplerate = 44100;
////        _audio.transcode( "/tmp/out.aac", _type );
////    }

////    { //repoen file
////        Audio _audio;
////        std::fstream _fs ( "/tmp/out.aac" );
////        _audio.input( &_fs );

////        EXPECT_EQ ( 1, _audio.nb_streams() );

////        MediaInfo _type = _audio.media_info( 0 );
////        EXPECT_EQ ( 1, _audio.nb_streams() );
////        EXPECT_EQ ( MediaType::AUDIO, _type.type );
////        EXPECT_EQ ( Codec::FLAC, _type.codec );
////        EXPECT_EQ ( 0 /*TODO*/, _type.bitrate );
////        EXPECT_EQ ( 44100, _type.samplerate );
////        EXPECT_EQ ( 2, _type.channels );
////        EXPECT_EQ ( 9078720, _type.playtime );
////        EXPECT_EQ ( 0, _type.bits_per_sample ); //TODO
////        EXPECT_EQ ( 0, _type.width );
////        EXPECT_EQ ( 0, _type.height );

////        EXPECT_EQ ( 11U, _type.metadata.tag_names().size() );
////        EXPECT_EQ ( "Victorious Love - Carolyn Sampson sings Purcell", _type.metadata.get( Metadata::ALBUM ) );
////        EXPECT_EQ ( "Sampson, Carolyn;Cummings, Laurence;Kenny, Elizabeth;Lasla, Anne-Marie", _type.metadata.get( Metadata::ARTIST ) );
////        EXPECT_EQ ( "Downloaded from eClassical.com. From album BIS-SACD-1536", _type.metadata.get( Metadata::COMMENT ) );
////        EXPECT_EQ ( "Purcell, Henry", _type.metadata.get( Metadata::COMPOSER ) );
////        EXPECT_EQ ( "1/1", _type.metadata.get( Metadata::DISC) );
////        EXPECT_EQ ( "(32)Vocal/Choral;(32)Baroque", _type.metadata.get( Metadata::GENRE ) );
////        EXPECT_EQ ( "Sampson, Carolyn;Cummings, Laurence;Kenny, Elizabeth;Lasla, Anne-Marie", _type.metadata.get( Metadata::PERFORMER ) );
////        EXPECT_EQ ( "BIS;eClassical", _type.metadata.get( Metadata::PUBLISHER ) );
////        EXPECT_EQ ( "(Sweeter than roses, Z.585 No.1) - Sweeter than roses, Z.585 No.1", _type.metadata.get( Metadata::TITLE ) );
////        EXPECT_EQ ( "1/19", _type.metadata.get( Metadata::TRACK ) );
////        EXPECT_EQ ( "2007-08-30", _type.metadata.get( Metadata::YEAR ) );
////    }
////}

////TEST( LibavTest, DISABLED_open_file_metadata ) {
////////    std::string filename = std::string(TESTFILES) + "BIS1536-001-flac_16.flac";
//////    std::string filename = std::string(TESTFILES) + "01 - I. Feierlich, misterioso.flac";


//////    Audio _audio( filename.c_str() );
//////    MediaInfo _type = _audio.media_info( 0 );

//////    EXPECT_EQ ( 11U, _type.metadata.tag_names().size() );

//////    EXPECT_EQ ( 6, _audio.nb_streams() );
//////    EXPECT_EQ ( MediaType::AUDIO, _type.type );
//////    EXPECT_EQ ( Codec::FLAC, _type.codec );
//////    EXPECT_EQ ( 0 /*TODO*/, _type.bitrate );
//////    EXPECT_EQ ( 44100, _type.samplerate );
//////    EXPECT_EQ ( 2, _type.channels );
//////    EXPECT_EQ ( 9078720, _type.playtime );
//////    EXPECT_EQ ( 0, _type.bits_per_sample );
//////    EXPECT_EQ ( 0, _type.width );
//////    EXPECT_EQ ( 0, _type.height );

////////TODO    _audio.transcode( "/home/e3a/01 - I. Feierlich, misterioso.mp3", MediaInfo { MediaType::AUDIO, Codec::MP3, 224000, 44100, 2, 0, 16, 0, 0, Metadata() } );

////////    Libav av ( filename.c_str() );
////////    av.transcode( "/home/e3a/out.mp3" );
//////    std::cout << "end" << std::endl;
////}

////TEST( LibavTest, open_stream_metadata ) {
//////    std::string filename = std::string(TESTFILES) + "BIS1536-001-flac_16.flac";
//////    std::string filename = std::string(TESTFILES) + "01 - I. Feierlich, misterioso.flac";

//////    std::unique_ptr< std::fstream > in = std::make_unique< std::fstream >( filename, std::ios_base::binary|std::ios_base::in );
//////    Audio _audio( in.get() );
//////    EXPECT_EQ ( 1, _audio.nb_streams() );
//////    MediaInfo _type = _audio.media_info( 0 );

//////    EXPECT_EQ ( 6U, _type.metadata.tag_names().size() );

//////    EXPECT_EQ ( 1, _audio.nb_streams() );
//////    EXPECT_EQ ( MediaType::AUDIO, _type.type );
//////    EXPECT_EQ ( Codec::FLAC, _type.codec );
//////    EXPECT_EQ ( 0 /*TODO*/, _type.bitrate );
//////    EXPECT_EQ ( 44100, _type.samplerate );
//////    EXPECT_EQ ( 2, _type.channels );
//////    EXPECT_EQ ( 74573100, _type.playtime );
//////    EXPECT_EQ ( 0, _type.bits_per_sample );
//////    EXPECT_EQ ( 0, _type.width );
//////    EXPECT_EQ ( 0, _type.height );

//////TODO    _audio.transcode( "/tmp/TEST_STREAM 01 - I. Feierlich, misterioso.mp3", MediaInfo { MediaType::AUDIO, Codec::MP3, 224000, 44100, 2, 0, 16, 0, 0, Metadata() } );

//////    Libav av ( filename.c_str() );
//////    av.transcode( "/home/e3a/out.mp3" );
////    std::cout << "end" << std::endl;
////}


//}//namespace libaav
