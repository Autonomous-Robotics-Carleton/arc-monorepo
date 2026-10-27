from glob import glob

from setuptools import setup

package = 'arc_car_config'
# The car files live at the repo root, config/cars/, since the firmware build
# reads them too; they're installed with this package for the nodes.
cars = '../../../config/cars'

setup(
    name=package,
    version='0.0.0',
    packages=[package],
    data_files=[
        ('share/ament_index/resource_index/packages', [f'resource/{package}']),
        (f'share/{package}', ['package.xml']),
        (f'share/{package}/cars', glob(f'{cars}/*.yaml') + [f'{cars}/schema.json']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='ARC',
    maintainer_email='devlead@arcarleton.ca',
    description='The car configuration: loader and checks',
    license='MIT',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'car_config = arc_car_config.cli:main',
        ],
    },
)
