#ifndef FILETYPES_H
#define FILETYPES_H

#include <QStringList>

namespace FileTypes {
    const QStringList fileTypes = {
        "*.jpg",  "*.JPG",
        "*.jpeg", "*.JPEG",
        "*.jpe",  "*.JPE",
        "*.png",  "*.PNG",
        "*.gif",  "*.GIF",
        "*.svg",  "*.SVG",
        "*.bmp",  "*.BMP",
        "*.pbm",  "*.PBM",
        "*.pgm",  "*.PGM",
        "*.ppm",  "*.PPM",
        "*.xbm",  "*.XBM",
        "*.xpm",  "*.XPM",
        // Plugin support
        "*.webp", "*.WEBP",
        "*.tiff", "*.TIFF",
        // RAW file types
        "*.cr2",  "*.CR2",
        "*.nef",  "*.NEF",
        "*.dng",  "*.DNG",
        // Add more file types
    };
}

#endif // FILETYPES_H
