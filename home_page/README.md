#TinyBlog
#####A minimal Django app for displaying a blog in your web project. Posts can feature images, render equations.

This app depends on

[Lightbox v2](http://lokeshdhakar.com/projects/lightbox2/), licensed under the Creative Commons Attribution 2.5 License

[Zocial CSS buttons](https://github.com/samcollins/css-social-buttons), licensed under the MIT License

[jQuery](www.jquery.com), licensed under the MIT License

[highlight.js](https://highlightjs.org/), licensed under the BSD License

##Settings which have to be added to the project
```python
BLOG_PAGESIZE = 5 #can be any positive integer
MEDIA_URL = '/media/'
MEDIA_ROOT = os.path.join(BASE_DIR, 'media')

STATIC_URL = '/static/'
STATICFILES_DIRS = (
    os.path.join(BASE_DIR, 'static'),
)

TEMPLATE_DIRS = (
    os.path.join(BASE_DIR,  'templates'),
)
```

##Add the app to the project urls.py file
The following (or something to the same effect) should be added to the project's urls.py file:
`url(r'^blog/', include('blog.urls', namespace='blog')),`

##Activating Disqus
Code for embedding Disqus is located in `templates/blog/post.html` and `templates/blog/list_pages.html` in comment 
blocks. Uncomment the code blocks and add your Disqus shortname and you're good to go.
