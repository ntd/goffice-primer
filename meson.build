project('goffice-primer', 'c',
        version: '0.1.0',
        license: 'GPL3')

goffice_dep = dependency('libgoffice-0.10')
gtk_dep     = dependency('gtk+-3.0')

executable('chart',
           sources: files([ 'chart.c' ]),
           dependencies: [ goffice_dep, gtk_dep ],
           install: false)
