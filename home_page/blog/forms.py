__author__ = 'Nelarius'

from django import forms

from blog.models import Post, Image


class PostForm(forms.Form):
    title = forms.CharField(label='Title')
    body = forms.CharField(label="Body text", widget=forms.Textarea)
    tease = forms.CharField(
        label="Synopsis: ",
        help_text="this text will be visible on the main page.",
        widget=forms.Textarea
    )
    status = forms.ChoiceField(choices=Post.STATUS_CHOICES)
    tags = forms.CharField(label="Tags: ", help_text="separate the tags with spaces", required=False)
    category = forms.CharField(label="Category", required=False)


class ImageForm(forms.ModelForm):
    class Meta:
        model = Image
        fields = ('image',)

