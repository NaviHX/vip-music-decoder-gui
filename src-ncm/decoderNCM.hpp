#ifndef __DECODERNCM_HPP__
#define __DECODERNCM_HPP__

#include "aes.h"
#include "cJSON.h"
#include "base64.h"
#include <cstdio>
#include <iostream>

// For MetaData
#include <taglib/mpegfile.h>
#include <taglib/flacfile.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/id3v2tag.h>
#include <taglib/tag.h>

namespace dec
{
    static void aesEcbDecrypt(const unsigned char *key, std::string &src, std::string &dst)
    {
        int n, i;

        unsigned char out[16];

        n = src.length() >> 4;

        dst.clear();

        AES aes(key);

        for (i = 0; i < n - 1; i++)
        {
            aes.decrypt((unsigned char *)src.c_str() + (i << 4), out);
            dst += std::string((char *)out, 16);
        }

        aes.decrypt((unsigned char *)src.c_str() + (i << 4), out);
        char pad = out[15];
        if (pad > 16)
        {
            pad = 0;
        }
        dst += std::string((char *)out, 16 - pad);
    }

    class metadata
    {
    private:
        std::string mAlbum;
        std::string mArtist;
        std::string mFormat;
        std::string mName;
        cJSON *mRaw;
        int mDuration;
        int mBirate;

    public:
        const std::string &getAlbum() { return mAlbum; }
        const std::string &getArtist() { return mArtist; }
        const std::string &getFormat() { return mFormat; }
        const std::string &getName() { return mName; }
        const int getDuration() { return mDuration; }
        const int getBirate() { return mBirate; }
        metadata(cJSON *raw)
        {
            if (!raw)
                return;
            cJSON *temp;
            int len, i;
            mRaw = raw;

            temp = cJSON_GetObjectItem(raw, "musicName");
            if (temp)
                mName = std::string(cJSON_GetStringValue(temp));

            temp = cJSON_GetObjectItem(raw, "album");
            if (temp)
                mAlbum = std::string(cJSON_GetStringValue(temp));

            temp = cJSON_GetObjectItem(raw, "artist");
            //if(temp)
            //    mArtist=std::string(cJSON_GetStringValue(temp));
            if (temp)
            {
                len = cJSON_GetArraySize(temp);
                for (i = 0; i < len - 1; i++)
                {
                    mArtist += std::string(cJSON_GetStringValue(cJSON_GetArrayItem(cJSON_GetArrayItem(temp, i), 0)));
                    mArtist += '/';
                }
                mArtist += std::string(cJSON_GetStringValue(cJSON_GetArrayItem(cJSON_GetArrayItem(temp, i), 0)));
            }

            temp = cJSON_GetObjectItem(raw, "birate");
            if (temp)
                mBirate = temp->valueint;

            temp = cJSON_GetObjectItem(raw, "duration");
            if (temp)
                mDuration = temp->valueint;

            temp = cJSON_GetObjectItem(raw, "format");
            if (temp)
                mFormat = std::string(cJSON_GetStringValue(temp));
        }
        ~metadata()
        {
            cJSON_Delete(mRaw);
        }
    };
    class decoderNCM
    {
    private:
        const unsigned char coreKey[17] = {0x68, 0x7A, 0x48, 0x52, 0x41, 0x6D, 0x73, 0x6F, 0x35, 0x6B, 0x49, 0x6E, 0x62, 0x61, 0x78, 0x57, 0};
        const unsigned char modifyKey[17] = {0x23, 0x31, 0x34, 0x6C, 0x6A, 0x6B, 0x5F, 0x21, 0x5C, 0x5D, 0x26, 0x30, 0x55, 0x3C, 0x27, 0x28, 0};
        const unsigned char mPNG[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
        enum ncmFormat
        {
            MP3,
            FLAC
        };
        std::string ipath;
        std::string opath;
        ncmFormat mFormat;
        std::string mimgData;
        FILE *mFile;
        unsigned char mKeyBox[256];
        metadata *mMetadata;

        bool isNcmFile()
        {
            unsigned int header;
            fread(&header, sizeof(unsigned int), 1, mFile);
            if (header != (unsigned int)0x4e455443)
                return false;
            fread(&header, sizeof(unsigned int), 1, mFile);
            if (header != (unsigned int)0x4d414446)
                return false;
            return true;
        }
        void buildKeyBox(unsigned char *key, int keyLen)
        {
            int i;
            for (i = 0; i < 256; i++)
            {
                mKeyBox[i] = (unsigned char)i;
            }
            unsigned char temp = 0;
            unsigned char c = 0;
            unsigned char lastByte = 0;
            unsigned char keyOffset = 0;
            for (i = 0; i < 256; i++)
            {
                temp = mKeyBox[i];
                c = ((temp + lastByte + key[keyOffset++]) & 0xff);
                if (keyOffset >= keyLen)
                    keyOffset = 0;
                mKeyBox[i] = mKeyBox[c];
                mKeyBox[c] = temp;
                lastByte = c;
            }
            /* 
            for(i=0;i<256;i++)
            {
                printf("0x%x ",mKeyBox[i]);
            }
             */
        }
        std::string mimgType(std::string &data)
        {
            if (memcmp(data.c_str(), mPNG, 8) == 0)
                return "image/png";
            return "image/jpeg";
        }
        void addMetaData()
        {
            TagLib::File *audioFile;
            TagLib::Tag *tag;
            TagLib::ByteVector byteVector(mimgData.c_str(), mimgData.length());

            // Add Picture
            if (mFormat == ncmFormat::MP3)
            {
                audioFile = new TagLib::MPEG::File(opath.c_str());
                tag = dynamic_cast<TagLib::MPEG::File *>(audioFile)->ID3v2Tag(true);
                if (mimgData.length() > 0)
                {
                    TagLib::ID3v2::AttachedPictureFrame *frame = new TagLib::ID3v2::AttachedPictureFrame;
                    frame->setMimeType(mimgType(mimgData));
                    frame->setPicture(byteVector);
                }
            }
            else if (mFormat == ncmFormat::FLAC)
            {
                audioFile = new TagLib::FLAC::File(opath.c_str());
                tag = audioFile->tag();
                if (mimgData.length() > 0)
                {
                    TagLib::FLAC::Picture *cover = new TagLib::FLAC::Picture;
                    cover->setMimeType(mimgType(mimgData));
                    cover->setType(TagLib::FLAC::Picture::FrontCover);
                    cover->setData(byteVector);
                    dynamic_cast<TagLib::FLAC::File *>(audioFile)->addPicture(cover);
                }
            }

            // Other info
            if (mMetadata != NULL)
            {
                tag->setAlbum(TagLib::String(mMetadata->getAlbum(), TagLib::String::UTF8));
                tag->setArtist(TagLib::String(mMetadata->getArtist(), TagLib::String::UTF8));
                tag->setTitle(TagLib::String(mMetadata->getName(), TagLib::String::UTF8));
            }
            tag->setComment(TagLib::String("Dumped by ncmdump", TagLib::String::UTF8));

            // Save File
            audioFile->save();
        }

    public:
        const std::string &getIPath() { return ipath; }
        const std::string &getOPath() { return opath; }
        decoderNCM(std::string const &path)
        {
            if ((mFile = fopen(path.c_str(), "rb")) == 0)
                std::cerr << "Open File Error\n";

            ipath = path;

            if (!isNcmFile())
                std::cerr << "Wrong File Format\n";
            fseek(mFile, 2, SEEK_CUR);
            int i;
            unsigned int n;

            fread(reinterpret_cast<char *>(&n), sizeof(n), 1, mFile);
            char keydata[n];
            fread(keydata, sizeof(char), n, mFile);
            for (i = 0; i < n; i++)
                keydata[i] ^= 0x64;
            std::string rawKeyData(keydata, n);
            std::string mKeyData;
            aesEcbDecrypt(coreKey, rawKeyData, mKeyData);
            // 17 : (neteasemusicmusic) invalid key data
            buildKeyBox((unsigned char *)mKeyData.c_str() + 17, mKeyData.length() - 17);
            fread(reinterpret_cast<char *>(&n), sizeof(n), 1, mFile);

            {
                char modifyData[n];
                fread(modifyData, sizeof(char), n, mFile);

                for (i = 0; i < n; i++)
                {
                    modifyData[i] ^= 0x63;
                }

                std::string swapModifyData;
                std::string modifyOutData;
                std::string modifyDecryptData;

                swapModifyData = std::string(modifyData + 22, n - 22);

                // escape `163 key(Don't modify):`
                Base64::Decode(swapModifyData, modifyOutData);

                aesEcbDecrypt(modifyKey, modifyOutData, modifyDecryptData);

                // escape `music:`
                modifyDecryptData = std::string(modifyDecryptData.begin() + 6, modifyDecryptData.end());

                mMetadata = new metadata(cJSON_Parse(modifyDecryptData.c_str()));
            }
            fseek(mFile, 9, SEEK_CUR);
            fread(reinterpret_cast<char *>(&n), sizeof(n), 1, mFile);
            if (n > 0)
            {
                char *imageData = (char *)malloc(n);
                fread(imageData, sizeof(char), n, mFile);
                mimgData = std::string(imageData, n);
            }
            else
            {
                std::cerr << "No album picture : " << path.c_str() << std::endl;
            }
        }
        ~decoderNCM()
        {
            if (mMetadata)
                delete mMetadata;
            fclose(mFile);
        }
        void dump()
        {
            // opath = fileNameWithoutExt(ipath);
            opath=ipath.substr(0, ipath.rfind('.'));
            FILE *w = NULL;
            int n = 0x8000, i = 0;
            unsigned char buffer[n];
            while (!feof(mFile))
            {
                n = fread(buffer, sizeof(unsigned char), n, mFile);
                for (i = 0; i < n; i++)
                {
                    int j = (i + 1) & 0xff;
                    buffer[i] ^= mKeyBox[(mKeyBox[j] + mKeyBox[(mKeyBox[j] + j) & 0xff]) & 0xff];
                }
                if (w == NULL)
                {
                    if (buffer[0] == 0x49 && buffer[1] == 0x44 && buffer[2] == 0x33)
                    {
                        opath += ".mp3";
                        mFormat = ncmFormat::MP3;
                    }
                    else
                    {
                        opath += ".flac";
                        mFormat = ncmFormat::FLAC;
                    }
                    w = fopen(opath.c_str(), "wb");
                }
                fwrite(buffer, sizeof(unsigned char), n, w);
            }
            fflush(w);
            fclose(w);
            addMetaData();
            std::cout << opath << std::endl;
        }
    };
} // namespace dec

#endif