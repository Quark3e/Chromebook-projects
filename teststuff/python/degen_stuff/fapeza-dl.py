#!/usr/bin/env python3

import argparse
import os
from pathlib import Path
from typing import List
from lxml import html
import requests

class Fapeza:

    api: dict = {
        'index': 'https://fapeza.com/ajax/index/page-{page}/',
        'model': 'https://fapeza.com/ajax/model/{model}/page-{page}/',
        'random': 'https://fapeza.com/ajax/random/page-{page}/',
    }
    config: dict = {
        'debug': False,
        'limit': 10,
        'mode': '',
        'username': '',
        'verbose': False,
    }
    models: List[str] = []

    def __init__(self, config) -> None:
        self.config.update(config)

    def download_media(self, path: Path, uri: str):
        response = requests.get(uri, stream=True)
        if response.ok:
            with open(path, 'wb') as f:
                f.write(response.content)
        
        if self.config['verbose']:
            print(f'Successfully downloaded {os.path.basename(path)}')

    def get_images(self, model: str, page: int) -> List[str]:
        response = requests.get(str(self.api['model']).format(model = model, page = page))
        document = html.fromstring(response.content)
        return document.xpath('//img[@class="thumb_img"]/@src')

    def get_models(self, page) -> List[str]:
        match self.config['mode']:
            case 'random':
                model_path: str = 'https://fapeza.com/ajax/random/page-{page}/'

            case _:
                model_path: str = 'https://fapeza.com/ajax/index/page-{page}/'
            
        page = requests.get(model_path.format(page = page))
        tree = html.fromstring(page.content)
        models: List[str] = tree.xpath('//div[@class="post-avatar"]/a/@href')
        for i in range(len(models)):
            models[i] = os.path.basename(models[i].rstrip('/'))
        return models

    def has_content(self, model: str, index: int):
        response = requests.get(str(self.api['model']).format(model = model, page = index))
        if response.content == b'':
            return False
        return True

    def process_model(self, model: str):
        print(f'Downloading {model}')
        if not os.path.exists(model):
            os.makedirs(model)
        model_path: Path = Path().absolute().joinpath(model)
        
        index = 0
        while self.has_content(model, index):
            images: List[str] = self.get_images(model, index)
            for image in images:
                image_uri = image.replace('_400px', '')
                image_path = model_path.joinpath(os.path.basename(image_uri))
                if not os.path.exists(image_path):
                    self.download_media(image_path, image_uri)
                else:
                    if self.config['verbose']:
                        print(f'Skipped... {image_path} already exists.')
            
            index += 1
        print(f'Download for {model} complete')
    
    def process_models(self):
        for model in self.models:
            self.process_model(model)

    def remove_duplicates(self):
        self.models = list(dict.fromkeys(self.models))
        self.models.sort()

    def run(self):
        index = 0

        if self.config['mode'] == 'single':
            if self.config['username'] == '':
                print('Username must be provided if running in single mode')
                exit(1)
            self.process_model(self.config['username'])
        else:
            while index in range(self.config['limit']):
                self.models += self.get_models(index)
                index += 1
            
            if self.config['debug']:
                print(self.models)

            self.remove_duplicates()
            for model in self.models:
                self.process_model(model)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-D', '--debug', help='Enable debug output')
    parser.add_argument('-v', '--verbose', help='Enables verbose output')
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('-d', '--default', action='store_true', help='Download models in default index')
    group.add_argument('-r', '--random', action='store_true', help='Download random models')
    group.add_argument('-s', '--single', action='store_true', help='Download single model by name')
    parser.add_argument('-l', '--limit', help='Default and Random mode page limit for downloads', default=10)
    parser.add_argument('-u', '--username', help='Single mode username for download')
    args = parser.parse_args()

    config: dict = {}
    if args.single:
        config['mode'] = 'single'
        if not args.username:
            parser.error('argument -u/--username is required in single mode')
        config['username'] = args.username
    if args.default or args.random:
        config['limit'] = args.limit
        if args.default:
            config['mode'] = 'default'
        if args.random:
            config['mode'] = 'random'
    
    if args.debug:
        config['debug'] = True
    if args.verbose:
        config['verbose'] = True
    
    app = Fapeza(config)
    app.run()

if __name__ == '__main__':
    main()

  