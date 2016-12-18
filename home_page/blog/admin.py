from django.contrib import admin
from blog.models import Post, Tag, Category, Image


class PostAdmin(admin.ModelAdmin):
    search_fields = ["title"]
    list_display = ('title', 'created', 'status')


class CategoryAdmin(admin.ModelAdmin):
    list_display = ('name',)

admin.site.register(Post, PostAdmin)
admin.site.register(Tag)
admin.site.register(Category, CategoryAdmin)
admin.site.register(Image)
