/**
 * projectM-qt -- Qt4 based projectM GUI 
 * Copyright (C)2003-2004 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */

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

	QString url, name;
	int rating, breedability;

	while (reader.readNext() != QXmlStreamReader::EndElement)
		if (reader.name() == "url") {
			bool repeat;
			int result;

			while (repeat = (result = reader.readNext()) == QXmlStreamReader::Characters) 	
				url += reader.text().toString();			
						
		} else if (reader.name() == "rating") {
			reader.readNext();
			rating = reader.text().toString().toInt();		
			reader.readNext();
		} else if (reader.name() == "breedability") {
			reader.readNext();
			breedability = reader.text().toString().toInt();
			reader.readNext();
		} else if (reader.name() == "name") {
			bool repeat;
			int result;

			while (repeat = (result = reader.readNext()) == QXmlStreamReader::Characters)	
				name += reader.text().toString();
		} else {
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

	readFunctor.appendItem(url, QFileInfo(url).fileName(), rating, breedability);	

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

	QString name;
	QString url;
	int rating;
	int breedability;
	
	while (writeFunctor.nextItem(name, url, rating, breedability)) {
		writer.writeStartElement("item");


		writer.writeStartElement("name");
		
		writer.writeCharacters(name.replace("&amp;", "&"));
		writer.writeEndElement();

		writer.writeStartElement("url");
		
		writer.writeCharacters(url.replace("&amp;", "&"));
		writer.writeEndElement();

		writer.writeStartElement("rating");
		writer.writeCharacters(QString("%1").arg(rating)+"\n");
		writer.writeEndElement();

		writer.writeStartElement("breedability");
		writer.writeCharacters(QString("%1").arg(breedability)+"\n");
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
