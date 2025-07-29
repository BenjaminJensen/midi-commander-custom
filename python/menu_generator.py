import yaml
from jinja2 import Environment, PackageLoader, select_autoescape
from datetime import datetime as dt

def ia_id_filter(input):
    string = "\", \"".join(input)
    return f'{{"{string}\", 0 }}'

def load_jinja_env():
    env = Environment(
        loader=PackageLoader("menu_generator"),
        autoescape=select_autoescape()
    )
    env.filters['ia_id_filter'] = ia_id_filter
    return env

def main():

    env = load_jinja_env()
    factory_preset = env.get_template("factory_settings.jinja")
    cfg = None
    with open("python/settings_meny.yaml") as f:
        cfg = yaml.load(f, Loader=yaml.SafeLoader)
    
    print(factory_preset.render(config=cfg, date=dt.now()))

if __name__ == "__main__":
    main()


"""
typedef struct {
  uint8_t *value,
  uint8_t max = 127,
  uint8_t min = 0,
  menu_type_t type = MENU_TYPE_NUMBER
} menu_pc_t;


"""