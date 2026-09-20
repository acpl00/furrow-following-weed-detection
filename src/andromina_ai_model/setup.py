from setuptools import find_packages, setup

package_name = 'andromina_ai_model'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='todo',
    maintainer_email='email="todo@todo.com"',
    description='TODO: Package description',
    license='Apache-2.0',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
           #'andromina_ai_model = andromina_ai_model.andromina_ai_model:main',
           #'andromina_ai_model = andromina_ai_model.andromina_ai_onnx_model:main',
           'andromina_ai_model = andromina_ai_model.andromina_ai_onnx_model_server:main',
        ],
    },
)
