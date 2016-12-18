from django.db import models


class Tag(models.Model):
    name = models.CharField(max_length=80, unique=True)

    def __str__(self):
        return self.name


class Category(models.Model):
    name = models.CharField(max_length=100, unique=True)
    slug = models.SlugField(max_length=100, unique=True)

    class Meta:
        verbose_name = 'Category'
        verbose_name_plural = 'Categories'

    def __str__(self):
        return self.name


class Post(models.Model):
    """
    The blog post model.
    """
    DRAFT = 1
    PUBLISH = 2
    STATUS_CHOICES = (
        (DRAFT, 'Draft'),
        (PUBLISH, 'Publish')
    )
    title = models.CharField(max_length=100)
    # The slug line is unique, because there are not going to be posts
    # with the same title
    slug = models.SlugField(unique=True, default='default_post')
    body = models.TextField()
    tease = models.TextField(blank=True, default='')
    created = models.DateTimeField(auto_now_add=True)
    status = models.IntegerField(choices=STATUS_CHOICES, default=PUBLISH)
    tags = models.ManyToManyField(Tag, blank=True, null=True)
    category = models.ForeignKey(Category, blank=True, null=True)

    def __str__(self):
        return self.title

    @property
    def get_tags(self):
        return self.tags.all()

    @property
    def tags_as_string(self):
        s = ""
        for tag in self.tags.all():
            s += tag.__str__() + " "
        return s[:len(s)-1]

    @property
    def category_as_string(self):
        if self.category.__str__ == "None":
            return ""
        else:
            return self.category.__str__()


class Image(models.Model):
    """
    A class to store and get the name of the image file.
    """
    image = models.FileField(upload_to='img/%Y/%m/%d')

    @property
    def filename(self):
        return self.image.name
