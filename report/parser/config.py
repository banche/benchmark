# Configuration for the benchmarks

class Description(object):

    def __init__(self, name, **kwargs):
        self.name = name

        # args parsed from kwargs
        self.global_timing = False
        self.description = ""
        self.details = ""

        print(kwargs.keys())
        if 'description' in kwargs.keys():
            self.description = kwargs['description']
        if 'details' in kwargs.keys():
            self.details = kwargs['details']

    def __str__(self):
        return '{ ' + self.name + ': ' + self.description + '}'

descriptions = dict()

rehash = Description('Rehash',
        global_timing = True,
        description = 'Rehash benchmark',
        details = 'Measures the time taken to insert an element which will lead to a rehash'
)


descriptions[rehash.name] = rehash
