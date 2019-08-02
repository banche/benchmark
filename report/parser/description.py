
class Description(object):
    """
    Simple wrapper to hold the benchmark description
    """
    def __init__(self, name, **kwargs):
        self.name = name

        # args parsed from kwargs
        self.global_timing = False
        self.description = None
        self.details = None
        self.legend = None

        if 'description' in kwargs.keys():
            self.description = kwargs['description']
        if 'details' in kwargs.keys():
            self.details = kwargs['details']
        if 'legend' in kwargs.keys():
            self.legend = kwargs['legend']

    def __str__(self):
        return '{ ' + self.name + ': ' + self.description + '}'
