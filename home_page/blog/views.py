import operator

from django.shortcuts import render_to_response, render
from django.core.paginator import Paginator, InvalidPage, EmptyPage
from django.core.urlresolvers import reverse
from django.http import HttpResponseRedirect
from django.conf import settings
from django.contrib.auth import logout
from django.contrib.auth.decorators import login_required
from django.template import RequestContext

from blog.models import Post, Tag, Category
from blog.forms import PostForm, ImageForm
from blog.search import get_query

from slugify import slugify


#TODO: add archive view to side bar
#TODO: add google analytics to website
#TODO: write backup scripts

def index(request):
    """
    The front page of the blog. Contains a list of posts,
    and the most popular tags

    Template: "blog/list_pages.html"

    Context:
        posts
            The list of posts, whose status=Post.PUBLISH
        tags
            The list of most popular tags
        sidebar
            The sidebar context variables that the base template uses
    """
    posts = Post.objects.filter(status=Post.PUBLISH).order_by("-created")
    paginator = Paginator(posts, settings.BLOG_PAGESIZE)

    try:
        page = int(request.GET.get("page", '1'))
    except ValueError:
        page = 1

    try:
        posts = paginator.page(page)
    except (InvalidPage, EmptyPage):
        posts = paginator.page(paginator.num_pages)

    return render_to_response(
        'blog/index.html',
        {'posts': posts, 'user': request.user, 'sidebar': sidebar_dictionary()},
        context_instance=RequestContext(request)
    )


def sidebar_dictionary():
    """
    Returns a dictionary with the following entries:
        months:
            list:
                tuple: (year(number), month(number), month name(string))
        tags:
            list:
                (tag(string))
        categories:
            list:
                tuple(name(string), slug(string))
    """
    variables = {}
    # Get the tag string of each post and count its occurrence
    tag_count = {}
    categories = []
    posts = Post.objects.filter(status=Post.PUBLISH).order_by("-created")
    for post in posts:
        try:
            categories.append((post.category.name, post.category.slug))
        except (Category.DoesNotExist, AttributeError):
            pass

        for tag in post.tags_as_string.split(' '):
            # Check for empty tag
            # An empty tag will be created when splitting an empty string, which will be
            # the case when the post does now contain any tags
            if tag != '':
                try:
                    tag_count[tag] += 1
                except KeyError:
                    tag_count[tag] = 1

    # Finally, sort the dictionary as a tuple, and take the first 10 values
    tags = sorted(tag_count.items(), key=operator.itemgetter(1), reverse=True)
    if len(tags) > 0:
        variables['tags'] = tags

    if len(categories) > 0:
        variables['categories'] = categories

    variables['recent_posts'] = posts[0:5]

    return variables


def about(request):
    return render_to_response(
        'blog/about.html',
        {'sidebar': sidebar_dictionary()},
        context_instance=RequestContext(request)
    )


def view_post(request, post_slug):
    """
    View a single post with comments and a comment form.
    :param request:
    :param post_slug:
    :return:
    """
    post = Post.objects.get(slug=post_slug)
    return render_to_response(
        'blog/post.html',
        {'post': post, 'user': request.user, 'sidebar': sidebar_dictionary()},
        context_instance=RequestContext(request)
    )


def add_post(request, post_slug=''):
    """
    The view for adding a new post.

    Template: "blog/add_post.html"

    Context:
        form
            The Post form that we want to save
        edit
            A boolean denoting whether we are in edit mode. The form action will then include the post_slug
        slug
            The post slug in order for the form action to redirect back here... with the post slug.

    :param request:
    :param post_slug: the optional argument for editing an existing post
    :return:
    """

    # Handle post upload
    if request.method == 'POST' and "submit_post" in request.POST:
        form = PostForm(request.POST)

        if form.is_valid():
            post = Post(
                title=form.cleaned_data['title'],
                body=form.cleaned_data['body'],
                tease=form.cleaned_data['tease'],
                status=form.cleaned_data['status'],
                slug=encode(form.cleaned_data['title'])
            )
            post.save()

            if form.cleaned_data['category'] != '':
                category_name = request.POST['category']
                category_slug = encode(category_name)
                post.category = Category.objects.get_or_create(name=category_name, slug=category_slug)[0]
                post.save()

            # Get the list of tags
            if request.POST['tags'] != '':
                tags = request.POST['tags']
                tag_list = tags.split(' ')
                for tag_string in tag_list:
                    # An extra space at the last tag will result in an empty tag
                    if tag_string != '':
                        tag = Tag.objects.get_or_create(name=tag_string)[0]
                        post.tags.add(tag)
                        post.save()

            return HttpResponseRedirect(reverse('blog:index'))
        else:
            print(form.errors)

    else:
        form = PostForm()

    return render_to_response(
        'blog/add_post.html',
        {
            'form': form,
            'sidebar': sidebar_dictionary()
        },
        context_instance=RequestContext(request)
    )


def upload_image(request):
    """
    A view for uploading a single image at a time, and displaying it's
    url.

    Template:
        blog/add_image_popup.html
        NOTE: this does not inherit from base.html!

    Context:
        form:
            an ImageForm instance
        url:
            a string representing the uploaded image's url


    :param request:
    :return:
    """

    code = ''
    if request.method == 'POST':
        image_form = ImageForm(request.POST, request.FILES)
        if image_form.is_valid():
            image = image_form.save()
            code = "<a href='{0}{1}' data-lightbox='ENTER GROUP' " \
                   "data-title='ENTER CAPTION'>" \
                   "<img class='thumbnail' src={0}{1} alt='test'>" \
                   "</a>".format(settings.MEDIA_URL, image.filename)
        else:
            print(image_form.errors)
    else:
        image_form = ImageForm()

    return render_to_response(
        'blog/add_image_popup.html',
        {'image_form': image_form, 'code': code},
        context_instance=RequestContext(request)
    )


def edit_post(request, post_slug):
    try:
        post = Post.objects.get(slug=post_slug)
    except Post.DoesNotExist:
        return HttpResponseRedirect(reverse('blog:add_post'))

    if request.method == 'POST':    # Save the data
        form = PostForm(request.POST)
        if form.is_valid():
            post.title = form.cleaned_data['title']
            post.body = form.cleaned_data['body']
            post.tease = form.cleaned_data['tease']
            post.status = form.cleaned_data['status']
            post.slug = encode(form.cleaned_data['title'])
            post.save()

            # Get the list of tags
            if request.POST['tags'] != '':
                tags = request.POST['tags']
                tag_list = tags.split(' ')
                for tag_string in tag_list:
                    if tag_string != '':
                        tag = Tag.objects.get_or_create(name=tag_string)[0]
                        post.tags.add(tag)
                        post.save()
            else:
                for tag in post.tags.all():
                    tag.post_set.remove(post)

            if request.POST['category'] != '':
                category_name = request.POST['category']
                category_slug = encode(category_name)
                category = Category.objects.get_or_create(name=category_name, slug=category_slug)[0]
                post.category = category
                post.save()
            else:
                # If the category was made blank, then dissociate the category from the post
                try:
                    post.category.post_set.remove(post)
                except AttributeError:
                    pass

            return HttpResponseRedirect(reverse('blog:index'))
        else:
            print(form.errors)
    else:   # Populate the form with data
        form = PostForm({
            'title': post.title,
            'body': post.body,
            'tease': post.tease,
            'status': post.status,
            'tags': post.tags_as_string,
            'category': post.category_as_string
        })

    return render_to_response(
        "blog/edit_post.html",
        {'post': post, 'form': form, 'sidebar': sidebar_dictionary()},
        context_instance=RequestContext(request)
    )


def delete_post(request, post_slug):
    try:
        post = Post.objects.get(slug=post_slug)
        tags = post.get_tags
        post.delete()
        for tag in tags:
            tag.count -= 1
            if tag.count == 0:
                Tag.objects.get(pk=tag.pk).delete()

    except Post.DoesNotExist:
        pass
    return HttpResponseRedirect(reverse('blog:index'))


def view_drafts(request):
    """
    View a list of posts, whose status is DRAFT.

    Template: "blog/view_drafts.html"

    Context:
        drafts
            the list of posts, whose status is Post.DRAFT
    :param request:
    :return:
    """

    posts = Post.objects.filter(status=Post.DRAFT).order_by("-created")
    paginator = Paginator(posts, settings.BLOG_PAGESIZE)

    try:
        page = int(request.GET.get("page", '1'))
    except ValueError:
        page = 1

    try:
        posts = paginator.page(page)
    except (InvalidPage, EmptyPage):
        posts = paginator.page(paginator.num_pages)

    return render_to_response(
        'blog/list_pages.html',
        {'posts': posts, 'user': request.user, 'sidebar': sidebar_dictionary()},
        context_instance=RequestContext(request)
    )


def view_tag(request, tag_name):
    """
    Template: "blog/view_tag.html"

    Context
        posts
            the list of posts that are being listed
        tag
            the tag object we are listing pages for

    :param request:
    :param tag_name:
    :return:
    """
    posts = []
    for post in Post.objects.all():
        try:
            post.tags.get(name=tag_name)
        except Tag.DoesNotExist:
            continue
        posts.append(post)
    paginator = Paginator(posts, settings.BLOG_PAGESIZE)
    try:
        page = int(request.GET.get('page', '1'))
    except ValueError:
        page = 1

    try:
        posts = paginator.page(page)
    except (InvalidPage, EmptyPage):
        posts = paginator.page(paginator.num_pages)

    tag = Tag.objects.get(name=tag_name)

    return render_to_response(
        'blog/view_tag.html',
        {'posts': posts, 'tag': tag, 'sidebar': sidebar_dictionary()},
        context_instance=RequestContext(request)
    )


def view_category(request, category_name):
    category = Category.objects.get(slug=category_name)
    posts = Post.objects.filter(category=category)

    paginator = Paginator(posts, settings.BLOG_PAGESIZE)
    try:
        page = int(request.GET.get('page', '1'))
    except ValueError:
        page = 1

    try:
        posts = paginator.page(page)
    except (InvalidPage, EmptyPage):
        posts = paginator.page(paginator.num_pages)

    return render_to_response(
        'blog/view_category.html',
        {'posts': posts, 'category': category, 'user': request.user, 'sidebar': sidebar_dictionary()}
    )


def search(request):
    query_string = ''
    found_entries = []

    if not request.method == 'POST':
        #if we've searched previously, then make this look like a post
        try:
            request.session['search-posts']
            request.method = 'POST'
            request.POST = request.session['search-posts']
        except KeyError:
            pass

    if request.method == 'POST':
        query_string = request.POST['q']
        request.session['search-posts'] = request.POST

        query = get_query(query_string, ['title', ])
        found_entries = Post.objects.filter(query).order_by('-created')

    paginator = Paginator(found_entries, settings.BLOG_PAGESIZE)

    try:
        page = int(request.GET.get("page", '1'))
    except ValueError:
        page = 1

    try:
        posts = paginator.page(page)
    except (InvalidPage, EmptyPage):
        posts = paginator.page(paginator.num_pages)

    return render_to_response(
        'blog/view_search.html',
        {'posts': posts, 'query_string': query_string, 'sidebar': sidebar_dictionary()},
        context_instance=RequestContext(request)
    )


@login_required
def user_logout(request):
    logout(request)
    return HttpResponseRedirect('/blog/')


def encode(string):
    """
    Replace the spaces with underscores. The slug fields use this scheme.
    :param string:
    :return:
    """
    return slugify(string, separator='_')
