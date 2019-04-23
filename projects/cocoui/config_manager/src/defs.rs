macro_rules! config {
    ( $dest_name:ident {
        $( $attr_name:ident : $attr_type:ty = $attr_default:expr),*
    })
    => {
        pub mod $dest_name {
            use std::fs;
            #[derive(Deserialize, Debug)]
            struct Optional {
                pub $( $attr_name : Option<$attr_type> ),*
            }

            impl Optional {
                fn empty() -> Optional {
                    Optional {
                        $( $attr_name : None ),*
                    }
                }
            }

            #[derive(Debug)]
            pub struct Config {
                $(pub $attr_name : $attr_type ),*
            }

            impl Config {
                fn read_from_file(file :&str) -> Option<Optional> {
                    match fs::read_to_string(file) {
                        Ok(ref s) => {
                            match toml::from_str(s) {
                                Ok(v) => Some(v),
                                _ => None,
                            }
                        },
                        _ => None,
                    }
                }

                pub fn new(name: &str) -> Config {
                    let global = format!("config/{}.toml", name);
                    let user = format!("config/{}.user.toml", name);
                    let global = match Config::read_from_file(&global) {
                        Some(v) => v,
                        None => Optional::empty(),
                    };
                    let user = match Config::read_from_file(&user) {
                        Some(v) => v,
                        None => Optional::empty(),
                    };
                    Config {
                        $( $attr_name: user.$attr_name.unwrap_or(global.$attr_name.unwrap_or($attr_default)),)*
                    }
                }
            }
        }
    }
}

#[derive(Deserialize,Debug)]
pub struct Board {
    pub id: u8,
    pub uid: String,
    pub name: String,
    pub path: Option<String>,
    pub simu: Option<String>,
}

config!(com {
    node_id: u8 = 0,
    auto_assign_id: bool = false,
    boards: Vec<super::Board> = vec![]
});


#[derive(Deserialize,Debug)]
pub struct FieldBorder {
    pub color: Vec<f64>,
    pub rect: Vec<f64>,
}


config!(field {
    background: String = String::new(),
    min_x: f64 = 0.0,
    min_y: f64 = 0.0,
    max_x: f64 = 1.0,
    max_y: f64 = 1.0,
    borders: Vec<super::FieldBorder> = vec![]
});


#[derive(Deserialize,Debug)]
pub struct Robot {
    pub shape: Vec<[f64; 2]>,
    pub fill: Vec<f64>,
    pub stroke: Vec<f64>,
}

impl Robot {
    pub fn new() -> Robot {
        Robot {
            shape: vec![],
            fill: vec![],
            stroke: vec![],
        }
    }
}

config!(robots {
    main: super::Robot = super::Robot::new(),
    pmi: super::Robot = super::Robot::new()
});
