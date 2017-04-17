#ifndef DESCRIPTION_H
#define DESCRIPTION_H

namespace upnp {

struct Description {
static http::http_status do_get( http::Request&, http::Response & response) {

    commons::xml::XMLWriter writer;
    commons::xml::Node root_node = writer.element( "root" );
    writer.ns( root_node, "urn:schemas-upnp-org:device-1-0" );
    writer.ns( root_node, "http://www.sec.co.kr/dlna", "sec" );

    commons::xml::Node spec_node = writer.element( root_node, "", "specVersion" );
    writer.element( spec_node, "", "major", "1" );
    writer.element( spec_node, "", "minor", "0" );

    commons::xml::Node device_node = writer.element( root_node, "", "device" );
    writer.element( device_node, "", "deviceType", "urn:schemas-upnp-org:device:MediaServer:1" );
    writer.element( device_node, "", "friendlyName", squawk::Config::instance()->name() );
    writer.element( device_node, "", "manufacturer", "spielhuus" );
    writer.element( device_node, "", "manufacturerURL", "http://www.spielhuus.org" );

    writer.element( device_node, "http://www.sec.co.kr/dlna", "ProductCap", "smi,DCM10,getMediaInfo.sec,getCaptionInfo.sec" );
    writer.element( device_node, "http://www.sec.co.kr/dlna", "ProductCap", "smi,DCM10,getMediaInfo.sec,getCaptionInfo.sec" );

    writer.element( device_node, "", "modelDescription", "Squawk DLNA Server" );
    writer.element( device_node, "", "modelName", "Windows Media Connect compatible (SquawkDLNA)" );
    writer.element( device_node, "", "modelNumber", "1" );
    writer.element( device_node, "", "modelURL", "http://" );
    writer.element( device_node, "", "serialNumber", "123456" );
    writer.element( device_node, "", "UDN", "uuid:" + squawk::Config::instance()->uuid() );

    commons::xml::Node dlna_node = writer.element( device_node, "", "X_DLNADOC", "DMS-1.50" );
    writer.ns( dlna_node, "urn:schemas-dlna-org:device-1-0", "dlna", true );

    writer.element( device_node, "", "presentationURL", "/" );

    commons::xml::Node icon_list_node = writer.element( device_node, "", "iconList" );
    commons::xml::Node icon_small_node = writer.element( icon_list_node, "", "icon" );
    writer.element( icon_small_node, "", "mimetype", "image/png" );
    writer.element( icon_small_node, "", "width", "48" );
    writer.element( icon_small_node, "", "height", "48" );
    writer.element( icon_small_node, "", "depth", "24" );
    writer.element( icon_small_node, "", "url", "/icons/squawk48.png" );

    commons::xml::Node icon_big_node = writer.element( icon_list_node, "", "icon" );
    writer.element( icon_big_node, "", "mimetype", "image/png" );
    writer.element( icon_big_node, "", "width", "64" );
    writer.element( icon_big_node, "", "height", "64" );
    writer.element( icon_big_node, "", "depth", "24" );
    writer.element( icon_big_node, "", "url", "/icons/squawk64.png" );

    commons::xml::Node icon_120_node = writer.element( icon_list_node, "", "icon" );
    writer.element( icon_120_node, "", "mimetype", "image/png" );
    writer.element( icon_120_node, "", "width", "120" );
    writer.element( icon_120_node, "", "height", "120" );
    writer.element( icon_120_node, "", "depth", "24" );
    writer.element( icon_120_node, "", "url", "/icons/squawk120.png" );

    commons::xml::Node icon_jpg_small_node = writer.element( icon_list_node, "", "icon" );
    writer.element( icon_jpg_small_node, "", "mimetype", "image/jpeg" );
    writer.element( icon_jpg_small_node, "", "width", "48" );
    writer.element( icon_jpg_small_node, "", "height", "48" );
    writer.element( icon_jpg_small_node, "", "depth", "24" );
    writer.element( icon_jpg_small_node, "", "url", "/icons/squawk48.jpg" );

    commons::xml::Node icon_jpg_big_node = writer.element( icon_list_node, "", "icon" );
    writer.element( icon_jpg_big_node, "", "mimetype", "image/jpeg" );
    writer.element( icon_jpg_big_node, "", "width", "64" );
    writer.element( icon_jpg_big_node, "", "height", "64" );
    writer.element( icon_jpg_big_node, "", "depth", "24" );
    writer.element( icon_jpg_big_node, "", "url", "/icons/squawk64.jpg" );

    commons::xml::Node icon_jpg_120_node = writer.element( icon_list_node, "", "icon" );
    writer.element( icon_jpg_120_node, "", "mimetype", "image/jpeg" );
    writer.element( icon_jpg_120_node, "", "width", "120" );
    writer.element( icon_jpg_120_node, "", "height", "120" );
    writer.element( icon_jpg_120_node, "", "depth", "24" );
    writer.element( icon_jpg_120_node, "", "url", "/icons/squawk120.jpg" );

    commons::xml::Node service_list_node = writer.element( device_node, "", "serviceList" );
    commons::xml::Node service_cds_node = writer.element( service_list_node, "", "service" );
    writer.element( service_cds_node, "", "serviceType", "urn:schemas-upnp-org:service:ContentDirectory:1" );
    writer.element( service_cds_node, "", "serviceId", "urn:upnp-org:serviceId:ContentDirectory" );
    writer.element( service_cds_node, "", "controlURL", "/ctl/ContentDir" );
    writer.element( service_cds_node, "", "eventSubURL", "/ctl/ContentDir" );
    writer.element( service_cds_node, "", "SCPDURL", "/ContentDir.xml" );

    commons::xml::Node service_cms_node = writer.element( service_list_node, "", "service" );
    writer.element( service_cms_node, "", "serviceType", "urn:schemas-upnp-org:service:ConnectionManager:1" );
    writer.element( service_cms_node, "", "serviceId", "urn:upnp-org:serviceId:ConnectionManager" );
    writer.element( service_cms_node, "", "controlURL", "/ctl/ConnectionMgr" );
    writer.element( service_cms_node, "", "eventSubURL", "/ctl/ConnectionMgr" );
    writer.element( service_cms_node, "", "SCPDURL", "/ConnectionMgr.xml" );

    /* TODO
    commons::xml::Node service_mrr_node = writer.element( service_list_node, "", "service" );
    writer.element( service_mrr_node, "", "serviceType", "urn:microsoft.com:service:X_MS_MediaReceiverRegistrar:1" );
    writer.element( service_mrr_node, "", "serviceId", "urn:microsoft.com:serviceId:X_MS_MediaReceiverRegistrar" );
    writer.element( service_mrr_node, "", "controlURL", "/ctl/X_MS_MediaReceiverRegistrar" );
    writer.element( service_mrr_node, "", "eventSubURL", "/ctl/X_MS_MediaReceiverRegistrar" );
    writer.element( service_mrr_node, "", "SCPDURL", "/X_MS_MediaReceiverRegistrar.xml" );
    */

    writer.element( device_node, "", "URLBase", "http://192.168.0.13:8080/" ); //TODO from config

    response << writer.str();
    response.parameter( http::header::CONTENT_TYPE, http::mime::mime_type( http::mime::XML ) );
    return( http::http_status::OK );
}
};
}//namespace upnp
#endif // DESCRIPTION_H
