macro_rules! config {
    ( $dest_name:ident {
        $( $attr_name:ident : $attr_type:ty = $attr_default:expr),*
    })
    => {
        pub mod $dest_name {
            use std::fs;
            #[derive(Deserialize)]
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
                        $( $attr_name: global.$attr_name.unwrap_or(user.$attr_name.unwrap_or($attr_default)),)*
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
    pub path: Option<String>,
}

config!(com {
    node_id: u8 = 0,
    auto_assign_id: bool = false,
    boards: Vec<super::Board> = vec![]
});

config!(eurobot {
    debug: u8 = 0
});
