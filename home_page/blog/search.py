__author__ = 'Nelarius'

import re

from django.db.models import Q


def normalize_query( query_string,
                     findterms=re.compile(r'"([^"]+)"|(\S+)').findall,
                     normspace=re.compile(r'\s{2,}').sub):
    """
    Splits the query string into individual keywords, getting rid of whitespace,
    and grouping quoted words together.
    :param query_string:
    :param findterms:
    :param normspace:
    :return:
    """
    return [normspace(' ', (t[0] or t[1]).strip()) for t in findterms(query_string)]


def get_query(query_string, search_fields):
    """
    Returns a query that is a combination of django Q objects. The query aims to search
    keywords within a model by testing the given search fields.
    :param query_string:
    :param search_fields:
    :return:
    """
    query = None
    terms = normalize_query(query_string)
    for term in terms:
        or_query = None

        for field_name in search_fields:
            q = Q(**{"{0}__icontains".format(field_name): term})
            if or_query is None:
                or_query = q
            else:
                or_query = or_query | q
        if query is None:
            query = or_query
        else:
            query = query & or_query
    return query
