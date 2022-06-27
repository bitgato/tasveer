<p align="center">
  <img width="100" src="images/Tasveer.png">
</p>
<h1 align="center">Tasveer</h1>
<p align="center">
  A simple image browsing and tagging application
</p>

## Description
Tasveer is a minimalistic Qt application made for browsing and tagging images
easily. It supports the most common file formats: `jpg`, `png`, `gif` and
`webp`.

## Screenshot
![Main window](images/screenshots/main.png)

## Features
- New directories can be added by the `File > Add Directories` menu.

- New single image can be added by the `File > Add Image` menu.

- New tags can be added by the `File > Add Tags` menu.
All characters are allowed except semicolons, which are used to separate tags.
In the dialog, enter the tags you want separated by semicolons. You'll see
buttons get added to the dialog as you type a tag and then a semicolon.
Note that even if you want to add a single tag, you'll have to end it with a
semicolon to actually add it.

![Add tags](images/screenshots/addtags.png)

- Tags are displayed on the left. These can be filtered by typing in the box
above the section. Filtering tags by multiple strings is possible by separating
the strings by semicolons. Two methods of tag filtering are available: `OR` and
`AND`.

![Filter tags by OR](images/screenshots/filtertagsor.png)
![Filter tags by AND](images/screenshots/filtertagsand.png)

- Added images are displayed on the right. These can be filtered by name by
typing in the box above the section.

![Filter tags by name](images/screenshots/filterimagesname.png)

- Tags can be linked to an image by dragging and dropping them on the desired
image. Multiple tags can be selected and dropped at once.

- Images can be filtered by tags by selecting desired tags and clicking the
`Filter images` button below the tag section. Two methods of filtering are
available: `OR` and `AND`.

![Filter images by tags OR](images/screenshots/filterimagesor.png)
![Filter images by tags AND](images/screenshots/filterimagesand.png)

- Images can be filtered by the directory they're in by selecting directories
in the dropdown menu above the images section.

- Tags linked to an image can be viewed by right clicking an image and
selecting `Show Tags`. A tag can be removed from an image by clicking on its
button and then confirming. The tags will not get removed unless you press
`OK` after removing desired tags.

![Show tags](images/screenshots/showtags.png)

- An image can be opened in the default image viewing application by
double-clicking on it.

- Images can be dragged and dropped to any location that supports dropping
images (file managers, browsers, etc).

## Shortcuts
- `Ctrl+D`: Open `Add directory` menu.
- `Ctrl+I`: Open `Add image` menu.
- `Ctrl+T`: Open `Add tags` menu.
- `Ctrl+S`: Set focus to tag filter search box.
- `Ctrl+Z`: Set focus to image filter search box.
- `Ctrl+Q`: Quit.

## Building and Installing
```bash
qmake && make
sudo make install
```

## Note
To deselect an item (tag or image), press `Ctrl` and click the left mouse
button.
