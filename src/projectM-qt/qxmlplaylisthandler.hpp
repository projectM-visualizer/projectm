#ifndef QXML_PLAYLIST_HANDLER_HPP
#define QXML_PLAYLIST_HANDLER_HPP
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class QXmlPlaylistHandler {

	public :
	template <class ReadFunctor>
	static QXmlStreamReader::Error readPlaylist (QIODevice * device, ReadFunctor & readFunc);

	template <class WriteFunctor>
	static void writePlaylist (QIODevice * device, WriteFunctor & writeFunc);

	private:
		
	template <class ReadFunctor>
		static QXmlStreamReader::Error readPlaylistItem(QXmlStreamReader & reader, ReadFunctor & functor);


};

template <class ReadFunctor>
QXmlStreamReader::Error QXmlPlaylistHandler::readPlaylist (QIODevice * device, ReadFunctor & readFunc)
{
	QXmlStreamReader reader ( device );

	QXmlStreamReader::TokenType token;

	try {
	while ( !reader.atEnd() ) {
		token = reader.readNext();
	switch ( token )
	{
		case QXmlStreamReader::StartElement:			

			if (reader.name() == "presetplaylist") {
			
			}
			else if (reader.name() == "description") {
				reader.readNext();
				readFunc.setPlaylistDesc(reader.text().toString().replace("&amp;", "&"));
				reader.readNext();
			}
			else if (reader.name() == "item") {
				readPlaylistItem<ReadFunctor>(reader, readFunc);
				break;
			} 
			break;
		case QXmlStreamReader::NoToken:
			break;
		case QXmlStreamReader::Invalid:
			break;
		case QXmlStreamReader::StartDocument:
			break;
		case QXmlStreamReader::EndDocument:
			break;
		case QXmlStreamReader::EndElement:	
			break;
		case QXmlStreamReader::Characters:
		case QXmlStreamReader::Comment:
		case QXmlStreamReader::DTD:
		case QXmlStreamReader::EntityReference:
		case QXmlStreamReader::ProcessingInstruction:
		default:
			break;
	}
	}
	} catch (const QXmlStreamReader::Error & id) {
		return id;
		
	}

	if (reader.hasError())
		return reader.error();
	 else
		return QXmlStreamReader::NoError;
}


template <class ReadFunctor>
QXmlStreamReader::Error QXmlPlaylistHandler::readPlaylistItem(QXmlStreamReader & reader, ReadFunctor & readFunctor) {

	QString url;
	int rating;

	while (reader.readNext() != QXmlStreamReader::EndElement)
		if (reader.name() == "url") {
			bool repeat;
			int result;

			while (repeat = (result = reader.readNext()) == QXmlStreamReader::Characters) {
						
			url += reader.text().toString();
			 			
			
			//qDebug() << "result:" << result;
			}
			
			
			//qDebug() << "read url is " << url;				
		} else if (reader.name() == "rating") {
			reader.readNext();
			rating = reader.text().toString().toInt();		
			reader.readNext();
		}
		else {			
			if (reader.name() == "")
				continue;
			else if (reader.hasError())
				return reader.error();
			else {
				try {		
					reader.raiseError(QString("Unknown element name %1 ").arg(reader.name().toString()));
				} catch (...) {
					return reader.error();
				}
			}
		}

	readFunctor.appendItem(url, QFileInfo(url).fileName(), rating);	

	return QXmlStreamReader::NoError;

}


template <class WriteFunctor>
void QXmlPlaylistHandler::writePlaylist (QIODevice * device, WriteFunctor & writeFunctor ) {


	QXmlStreamWriter writer(device);
	
	writer.writeStartDocument();

	writer.writeStartElement("presetplaylist");


 	writer.writeStartElement("description");
	writer.writeCharacters(writeFunctor.playlistDesc());
	writer.writeEndElement();


	QString url;
	int rating;

	while (writeFunctor.nextItem(url, rating)) {
		writer.writeStartElement("item");

		writer.writeStartElement("url");
		
		writer.writeCharacters(url.replace("&amp;", "&"));
		writer.writeEndElement();

		writer.writeStartElement("rating");
		writer.writeCharacters(QString("%1").arg(rating)+"\n");
		writer.writeEndElement();

		writer.writeEndElement();
	
	}

	writer.writeEndElement();

	writer.writeEndDocument();

	return;
}


/// @idea  Instead of templates, allow subclassing of the playlist handler
#if 0
virtual void QXmlPlaylistHandler::readPresetItem(const QString & url, int rating) {}


virtual bool QXmlPlaylistHandler::nextPresetItem(QString & url, int & rating) {
	return false;
}
#endif

#endif
