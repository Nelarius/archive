__author__ = 'Nelarius'

from django.conf.urls import patterns, url
from blog import views

urlpatterns = patterns('',
    url(r'^$', views.index, name='index'),
    url(r'^about/$', views.about, name='about'),
    url(r'^post/(?P<post_slug>\w+)/$', views.view_post, name='view_post'),
    url(r'^add_post/$', views.add_post, name='add_post'),
    url(r'^upload_image/$', views.upload_image, name='upload_image'),
    url(r'^edit/(?P<post_slug>\w+)/$', views.edit_post, name='edit_post'),
    url(r'^delete/(?P<post_slug>\w+)/$', views.delete_post, name='delete_post'),
    url(r'^tags/(?P<tag_name>\w+)/$', views.view_tag, name='view_tag'),
    url(r'^drafts/$', views.view_drafts, name='view_drafts'),
    url(r'^category/(?P<category_name>\w+)/$', views.view_category, name='view_category'),
    url(r'^search/$', views.search, name='search'),
    url(r'^logout&$', views.user_logout, name='logout'),
)