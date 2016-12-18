from django.shortcuts import render_to_response
from django.template import RequestContext

# Create your views here.


def index(request):
    return render_to_response(
        'home/home.html',
        context_instance=RequestContext(request)
    )


def cv(request):
    return render_to_response(
        'home/cv.html',
        context_instance=RequestContext(request)
    )

